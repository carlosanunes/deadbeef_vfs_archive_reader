/*
	RAR, 7z and Gzip archive reader
	Copyright (c) 2011-2016 Carlos Nunes <carloslnunes@gmail.com>.
	Uses the File_Extractor 1.0.0 library (C) Shay Green, http://code.google.com/p/file-extractor/

	RAR, 7z and Gzip archive reader is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	RAR, 7z and Gzip archive reader is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <deadbeef/deadbeef.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

//#define trace(...) { fprintf(stderr, __VA_ARGS__); }
#define trace(fmt,...)

#define min(x,y) ((x)<(y)?(x):(y))

#include "fex/fex/fex.h"

static DB_functions_t *deadbeef;
static DB_vfs_t plugin;

typedef struct {
    DB_FILE file;
    fex_t* archive;
    int64_t size;
    int64_t tell; // current position in file
} archive_file_t;


static const char *scheme_names[]    = { "rar://", "7z://", "gz://", "zip2://", NULL };
static const char *file_extensions[] = { ".rar",   ".7z",   ".gz",   ".zip2",   NULL };

const char **
vfs_archive_reader_get_schemes (void) {
    return scheme_names;
}

int
vfs_archive_reader_is_streaming (void) {
    return 0;
}

// fname must have form of rar://full_filepath.rar:full_filepath_in_rar
DB_FILE*
vfs_archive_reader_open (const char *fname) {

    int i = 0;
    while(1) {
        if (!scheme_names[i]) {
            return NULL;
        }
        size_t len = strlen(scheme_names[i]);
        if ( strncasecmp(fname, scheme_names[i], len) == 0 ) {
            fname += len; //remove rar://, 7z://, gz://, etc
            break;
        }
        i++;
    }

    const char *colon = strchr (fname, ':');
    if (!colon) {
        return NULL;
    }

    char archname[colon-fname+1];
    memcpy (archname, fname, colon-fname);
    archname[colon-fname] = 0;

    fname = colon+1;

        fex_t* fex;
        if (fex_open( &fex, archname) != NULL ) /* opening the rar */
                { fex_close(fex); return NULL; }

        /* the library doesn have a built in search function so */
        int n = 0; 
        while ( !fex_done( fex ) )
        {
                if(strcmp(fex_name(fex), fname) == 0 )
                        break; /* found the one */

                ++n;
                fex_next( fex );
        }
        //  are we at the end of the archive?
        if(fex_done(fex))
                { fex_close(fex); return NULL; }

        if (fex_stat(fex) != NULL)
                { fex_close(fex); return NULL; }

    archive_file_t *f = malloc (sizeof (archive_file_t));
    memset (f, 0, sizeof (archive_file_t));
    f->size = (int64_t) fex_size(fex);
    f->archive = fex;
    f->tell = 0;
    f->file.vfs = &plugin;      

    return (DB_FILE*)f;

}

void
vfs_archive_reader_close (DB_FILE *f) {
        archive_file_t *arc = (archive_file_t *)f;
        fex_close(arc->archive);
        free(arc);
}

size_t
vfs_archive_reader_read (void *ptr, size_t size, size_t nmemb, DB_FILE *f) {
        archive_file_t *arc = (archive_file_t *)f;

        int n = min ( size * nmemb, arc->size - arc->tell );
        
        // Fine to call fex_data() every time, since it internally caches data in memory
        // for ALL file types when you use fex_data().
        const void* data;
        if ( fex_data( arc->archive, &data ) != NULL )
                assert( 0 );
        
        memcpy( ptr, (const char*) data + arc->tell, n );
        arc->tell += n;

        // Do NOT free data since fex manages it internally
        

        return n/size;
}

int
vfs_archive_reader_seek (DB_FILE *f, int64_t offset, int whence) {
        archive_file_t *arc = (archive_file_t *)f;

    if (whence == SEEK_CUR) {
        offset += arc->tell;
    } else if (whence == SEEK_END) {
        offset += (int64_t) arc->size;
    }
        
        if ( offset < 0 || offset > arc->size )
                return -1;
        
        arc->tell = offset;
    return 0;
}

int64_t
vfs_archive_reader_tell (DB_FILE *f) {
    archive_file_t *arc = (archive_file_t *)f;
    return (int64_t) arc->tell;
}


void
vfs_archive_reader_rewind (DB_FILE *f) {
        archive_file_t *arc = (archive_file_t *)f;
        arc->tell = 0;
}


int64_t
vfs_archive_reader_getlength (DB_FILE *f) {
        archive_file_t *arc = (archive_file_t *)f;
    return arc->size;
}

/* getting the contents  */
int
vfs_archive_reader_scandir (const char *dir, struct dirent ***namelist, int (*selector) (const struct dirent *), int (*cmp) (const struct dirent **, const struct dirent **)) {
        
        fex_t* fex;
        if (fex_open( &fex, dir) != NULL )
                return -1;
        
        int n = 0;
        while ( !fex_done(fex) )
        {
                *namelist = realloc (*namelist, sizeof (void *) * (n+1) );
                (*namelist)[n] = malloc (sizeof (struct dirent));
                memset ((*namelist)[n], 0, sizeof (struct dirent));

                snprintf ((*namelist)[n]->d_name, sizeof ((*namelist)[n]->d_name), "%s", fex_name(fex) );

                fex_next(fex);
                ++n;
        }

        fex_close(fex);

        return n;
}


int
vfs_archive_reader_is_container (const char *fname) {
    const char *ext = strrchr (fname, '.');
    if (ext) {
        int i;
        for (i = 0; file_extensions[i]; i++ ) {
            if ( !strcasecmp (ext, file_extensions[i]) ) {
                return 1;  // ok
            }
        }
    }
    return 0;
}

const char *
vfs_archive_reader_get_scheme_for_name (const char * fname) {
    const char *ext = strrchr (fname, '.');
    if (ext) {
        int i;
        for (i = 0; file_extensions[i]; i++ ) {
            if ( !strcasecmp (ext, file_extensions[i]) ) {
                return scheme_names[i]; // ok
            }
        }
    }
    return NULL;
}

/* boilerplate */

static DB_vfs_t plugin = {
    .plugin.api_vmajor = 1,
    .plugin.api_vminor = 6,
    .plugin.version_major = 1,
    .plugin.version_minor = 1,
    .plugin.type = DB_PLUGIN_VFS,
    .plugin.id = "vfs_archive_reader",
    .plugin.name = "RAR, 7z and Gzip archive reader",
    .plugin.descr = "play files directly from rar,7z and gzip archive files",
    .plugin.copyright = 
        "Copyright (c) 2011-2016 Carlos Nunes <carloslnunes@gmail.com>.\n"
        "Uses the File_Extractor 1.0.0 library (C) Shay Green, http://code.google.com/p/file-extractor/ \n"
        "\n"
        "RAR, 7z and Gzip archive reader is free software; you can redistribute it and/or \n"
        "modify it under the terms of the GNU Lesser General Public\n"
        "License as published by the Free Software Foundation; either\n"
        "version 2.1 of the License, or (at your option) any later version.\n"
        "\n"
        "RAR, 7z and Gzip archive reader is distributed in the hope that it will be useful,\n"
        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
        "Lesser General Public License for more details.\n"
        "\n"
        "You should have received a copy of the GNU Lesser General Public\n"
        "License along with this library; if not, write to the Free Software\n"
        "Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA\n",
    .plugin.website = "http://subversion.assembla.com/svn/deadbeef_vfs_archive_reader/",
    .open = vfs_archive_reader_open,
    .close = vfs_archive_reader_close,
    .read = vfs_archive_reader_read,
    .seek = vfs_archive_reader_seek,
    .tell = vfs_archive_reader_tell,
    .rewind = vfs_archive_reader_rewind,
    .getlength = vfs_archive_reader_getlength,
    .get_schemes = vfs_archive_reader_get_schemes,
    .is_streaming = vfs_archive_reader_is_streaming,
    .is_container = vfs_archive_reader_is_container,
    .scandir = vfs_archive_reader_scandir,
    .get_scheme_for_name = vfs_archive_reader_get_scheme_for_name,
};

DB_plugin_t *
ddb_archive_reader_load (DB_functions_t *api) {

    deadbeef = api;
    return DB_PLUGIN (&plugin);
}
