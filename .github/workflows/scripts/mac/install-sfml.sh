#!/bin/sh -ve

# brew install SFML does not install it as frameworks,
# so instead we manually download the release from GitHub
# and copy the files to an appropriate location.

curl -LO https://github.com/SFML/SFML/releases/download/2.5.1/SFML-2.5.1-macOS-clang.tar.gz
tar -xzf SFML-2.5.1-macOS-clang.tar.gz
cp -r SFML-2.5.1-macOS-clang/Frameworks/* SFML-2.5.1-macOS-clang/extlibs/* $(brew --prefix)/Frameworks
