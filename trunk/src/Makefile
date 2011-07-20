CC = gcc
CFLAGS = -std=c99 -shared -O2 -fpic -lz -Wall

all: vfs_archive_reader.so

vfs_archive_reader.so: vfs_archive_reader_fast.c libfex.a
	$(CC) $(CFLAGS) vfs_archive_reader_fast.c libfex.a -o vfs_archive_reader.so

install: all
	mv vfs_archive_reader.so ~/.local/lib/deadbeef/

uninstall: 
	rm ~/.local/lib/deadbeef/vfs_archive_reader.so

clean:
	rm -rf vfs_archive_reader.so
