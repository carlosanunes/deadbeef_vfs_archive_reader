#!/bin/bash
set -e # exit with nonzero exit code if anything fails


case "$TRAVIS_OS_NAME" in
	linux)
		echo Uploading linux build
	;;
	osx)
		echo Upload osx build
	;;
esac