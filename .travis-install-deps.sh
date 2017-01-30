#!/bin/bash

set -ev

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
	# Also needs boost, but apparently that's already installed in Travis
	brew install scons sfml
else
	sudo apt-get update -qq
	sudo apt-get install -qq libsfml-dev libboost-dev
fi
