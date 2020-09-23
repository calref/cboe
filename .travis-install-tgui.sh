#!/bin/bash

set -ev

env GIT_SSL_NO_VERIFY=true  git clone -b 0.8 --single-branch https://github.com/texus/TGUI.git
cd TGUI && cmake . && make && sudo make install && cd /

