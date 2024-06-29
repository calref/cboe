#!/bin/sh -ve

git clone --depth 1 -b 0.9 https://github.com/texus/TGUI.git
cd TGUI
export CLICOLOR_FORCE=1
SFML_DIR=../deps/lib/cmake/SFML cmake -D TGUI_CXX_STANDARD=14 -D SFML_DIR=../deps/lib/cmake/SFML .
make
cmake --install .
cd .. # Probably not needed but...
