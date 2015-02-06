#!/bin/bash

function invalid {
    echo "$1 is not a valid Blades of Exile scenario."
    echo $2
    rm "$1.temp"
    exit 1
}

BOE_SCHEMAS_DIR=${BOE_SCHEMAS_DIR:-`git rev-parse --show-toplevel`/rsrc/boes}

if [[ -d $1 ]]; then
	SCEN_PATH=$1
elif [[ -f $1 ]]; then
	gzip -S .boes -dck "$1" > "$1.temp"
	if [[ $? > 0 ]]; then invalid "$1" "(Not valid GZIP data.)"; fi
	tar -xf "$1.temp"
	if [[ $? > 0 ]]; then invalid "$1" "(Not valid TAR archive.)"; fi
	rm "$1.temp"
	SCEN_PATH=scenario
else invalid "$1" "(Not a file or directory.)"; fi

function check-file {
	xmllint --nonet --noout --schema "$BOE_SCHEMAS_DIR/$1.xsd" "$2"
}

check-file scenario "$SCEN_PATH/scenario.xml"
check-file terrain "$SCEN_PATH/terrain.xml"
check-file items "$SCEN_PATH/items.xml"
check-file monsters "$SCEN_PATH/monsters.xml"

shopt -s nullglob

for sector in $SCEN_PATH/out/out*.xml; do
	check-file outdoor "$sector"
done

for town in $SCEN_PATH/towns/town*.xml; do
	check-file town "$town"
done

for speech in $SCEN_PATH/towns/talk*.xml; do
	check-file dialogue "$speech"
done

if [[ $SCEN_PATH = scenario ]]; then
	rm -rf scenario
fi
