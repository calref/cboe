#!/bin/sh
# Usage: pack-scenario.sh scenario-folder scenario-filename

rm -f "$2"
cd "$1/.."
mv "$1" scenario
tar -czf "$2" scenario
mv scenario "$1"

