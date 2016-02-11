#!/bin/bash
set -e # exit with nonzero exit code if anything fails

case "$TRAVIS_OS_NAME" in
	linux)
		cd build/linux
	;;
	osx)
		cd build/osx
	;;
esac

# dummy repo
git init

# inside this git repo we'll pretend to be a new user
git config user.name "Travis CI"
git config user.email "dummy@email.com"

git add .
git commit -m "Build push"

case "$TRAVIS_OS_NAME" in
	linux)
		echo Uploading linux build
		git push --force --quiet "https://${GH_TOKEN}@${GH_REF}" master:linux_releases > /dev/null 2>&1
	;;
	osx)
		echo Uploading osx build
		git push --force --quiet "https://${GH_TOKEN}@${GH_REF}" master:osx_releases > /dev/null 2>&1
	;;
esac

