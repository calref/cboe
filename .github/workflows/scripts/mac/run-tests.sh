#!/bin/sh -ve

cd test

# These are the same command-line arguments that are passed in the Xcode scheme.
# If the scheme is changed, these should be updated too.
../proj/xc$1/build/Release/boe_test -i --order lex
