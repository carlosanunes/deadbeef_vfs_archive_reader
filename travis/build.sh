case "$TRAVIS_OS_NAME" in
    linux)
        echo "installing the needed build dependencies..."
        sudo apt-get update 1> /dev/null 2> /dev/null || exit 1
        sudo apt-get install -qq libc6-dev-i386 libc6-dev 1> /dev/null 2> /dev/null || exit 1
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