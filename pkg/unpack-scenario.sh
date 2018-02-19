#!/bin/sh
# Usage: unpack-scenario.sh scenario-filename scenario-folder

rm -f "$2"
tar -xzf "$1"
mv scenario "$2"

