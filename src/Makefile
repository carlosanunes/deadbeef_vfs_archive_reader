CC?=gcc
ZLIB_LIBS?=-lz
CFLAGS+=-std=c99 -fPIC -Wall
LDFLAGS+=-shared -lm $(ZLIB_LIBS) -I.
MAKE=make

OS :=$(shell uname)
ifeq ($(OS), Darwin)
OUT=ddb_archive_reader.dylib
BUILDDIR=../build/osx
else
OUT=ddb_archive_reader.so
BUILDDIR=../build/linux
endif

all: $(OUT)

$(OUT): vfs_archive_reader.c libfex.a build_dir
	$(CC) $(CFLAGS) vfs_archive_reader.c libfex.a $(LDFLAGS) -o $(BUILDDIR)/$(OUT)

build_dir:
	mkdir -p $(BUILDDIR)

libfex.a: libfex
	mv fex/libfex.a .

libfex:
	$(MAKE) -C fex/ -f makefile	libfex.a

install: all
	mv $BUILDDIR/$(OUT) ~/.local/lib/deadbeef/

uninstall: 
	rm ~/.local/lib/deadbeef/$(OUT)

clean:
	rm -rf *.a $(BUILDDIR)
