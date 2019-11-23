#!/bin/bash

set -ev

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
	# Also needs boost, but apparently that's already installed in Travis
	HOMEBREW_NO_AUTO_UPDATE=1 brew install scons sfml
else
	sudo apt-get update -qq
	sudo apt-get install -qq libsfml-dev libboost-dev libboost-filesystem-dev libboost-thread-dev libxml2-utils
fi
