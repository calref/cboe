#!/bin/bash

REVID=`/opt/local/bin/git rev-parse HEAD`;
export FULLTAG=`/opt/local/bin/git tag --sort=v:refname | tail -n1`;
TAGREV=`/opt/local/bin/git rev-parse $FULLTAG`;

echo -e "\n#define GIT_REVISION \"${REVID:0:7}\"\n#define GIT_TAG \"$FULLTAG\"\n#define GIT_TAG_REVISION \"${TAGREV:0:7}\"\n" > tools/gitrev.hpp
