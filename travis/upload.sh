#!/bin/bash
set -e # exit with nonzero exit code if anything fails

echo uploading $TRAVIS_OS_NAME build
case "$TRAVIS_OS_NAME" in
	linux)
		cd build/linux
		curl -u "${BOX_USERNAME}:${BOX_PASSWORD}" -T ddb_archive_reader_i686.tar.bz2 https://dav.box.com/dav/ddb_archive_reader_i686.tar.bz2 > /dev/null 2>&1
		curl -u "${BOX_USERNAME}:${BOX_PASSWORD}" -T ddb_archive_reader_x86_64.tar.bz2 https://dav.box.com/dav/ddb_archive_reader_x86_64.tar.bz2 > /dev/null 2>&1
	;;
	osx)
		cd build/osx
		curl -u "${BOX_USERNAME}:${BOX_PASSWORD}" -T ddb_archive_reader_osx.zip https://dav.box.com/dav/ddb_archive_reader_osx.zip > /dev/null 2>&1
	;;
esac
