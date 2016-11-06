#!/bin/bash

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
	brew install scons sfml boost
else
	sudo apt-get update -qq
	sudo apt-get install -qq libsfml-dev libboost-dev
fi
