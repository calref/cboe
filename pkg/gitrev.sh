#!/bin/bash
# Usage: gitrev.sh path-to-src

# This script generates the header with git revision information
# NOTE: Changes made here must also be made in SConstruct!
REVID=`git rev-parse HEAD`;
export FULLTAG=`git tag --sort=v:refname | tail -n1`;
TAGREV=`git rev-parse $FULLTAG`;
STATUS=`git diff-index --quiet HEAD -- && echo "clean" || echo "modified"`;
REPO=`git remote get-url origin`;

echo -e "\n#define GIT_REVISION \"${REVID:0:7}\"\n#define GIT_TAG \"$FULLTAG\"\n#define GIT_TAG_REVISION \"${TAGREV:0:7}\"\n#define GIT_STATUS \"${STATUS}\"\n#define GIT_REPO \"${REPO}\"\n" > "$1/tools/gitrev.hpp"
