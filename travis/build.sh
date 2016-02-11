#!/bin/bash
case "$TRAVIS_OS_NAME" in
    linux)
        cd src
        echo "building for i686"
        ARCH=i686 make || exit 1
        echo "building for x86_64"
        ARCH=x86_64 make || exit 1
        cd ..
    ;;
    osx)
        cd src
        make || exit 1
        cd ..
    ;;
esac