#!/bin/sh

#  fix_dylibs.sh
#  BoE
#
#  Created by Celtic Minstrel on 14-04-17.
#

BOEPATH="$BUILT_PRODUCTS_DIR/$EXECUTABLE_PATH"
python3 "$SOURCE_ROOT/../../deps/fix-rpaths/fix-rpaths.py" "$BOEPATH"