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

echo uploading $TRAVIS_OS_NAME build
git push --force --quiet "https://${GH_TOKEN}@${GH_REF}" master:releases > /dev/null 2>&1
