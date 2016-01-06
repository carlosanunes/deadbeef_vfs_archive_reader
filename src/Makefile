CC?=gcc
ZLIB_LIBS?=-lz
CFLAGS+=-std=c99 -fPIC -Wall
LDFLAGS+=-shared -lm $(ZLIB_LIBS) -I.
MAKE=make

OS :=$(shell uname)
ifeq ($(OS), Darwin)
OUT=ddb_archive_reader.dylib
else
OUT=ddb_archive_reader.so
endif

all: $(OUT)

$(OUT): vfs_archive_reader.c libfex.a
	$(CC) $(CFLAGS) vfs_archive_reader.c libfex.a $(LDFLAGS) -o $(OUT)

libfex.a: libfex
	mv fex/libfex.a .

libfex:
	$(MAKE) -C fex/ -f makefile	libfex.a

install: all
	mv $(OUT) ~/.local/lib/deadbeef/

uninstall: 
	rm ~/.local/lib/deadbeef/$(OUT)

clean:
	rm -rf $(OUT)  *.a
