CC = gcc
CFLAGS = -std=c99 -shared -O2 -fpic -lz -Wall
MAKE = make

all: vfs_archive_reader.so

vfs_archive_reader.so: vfs_archive_reader.c libfex.a
	$(CC) $(CFLAGS) vfs_archive_reader.c libfex.a -o vfs_archive_reader.so

libfex.a: libfex
	mv fex/libfex.a .

libfex:
	$(MAKE) -C fex/ -f makefile	libfex.a

install: all
	mv vfs_archive_reader.so ~/.local/lib/deadbeef/

uninstall: 
	rm ~/.local/lib/deadbeef/vfs_archive_reader.so

clean:
	rm -rf vfs_archive_reader.so *.a
