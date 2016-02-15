#!/bin/bash
case "$TRAVIS_OS_NAME" in
    linux)
        cd src
        echo "building for i686"
        ARCH=i686 make || exit 1
        tar -cvjSf ../ddb_archive_reader_i686.tar.bz2  ddb_archive_reader.so || exit 1
        echo "building for x86_64"
        ARCH=x86_64 make || exit 1
        tar -cvjSf ../ddb_archive_reader_x86_64.tar.bz2  ddb_archive_reader.so || exit 1
        cd ..
    ;;
    osx)
        cd src
        make || exit 1
        zip -r ../ddb_archive_reader_osx.zip ddb_archive_reader.dylib || exit 1
        cd ..
    ;;
esac
