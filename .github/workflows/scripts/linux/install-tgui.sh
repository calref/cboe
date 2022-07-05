#!/bin/sh -ve

git clone --depth 1 -b 0.8 https://github.com/texus/TGUI.git
cd TGUI
export CLICOLOR_FORCE=1
cmake .
make
cmake --install .
cd .. # Probably not needed but...
