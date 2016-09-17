#!/bin/bash

function invalid {
    echo "$1 is not a valid Blades of Exile scenario."
    echo $2
    rm "$1.temp"
    exit 1
}

BOE_SCHEMAS_DIR=${BOE_SCHEMAS_DIR:-`git rev-parse --show-toplevel`/rsrc/schemas}

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

cd "$SCEN_PATH"

check-file scenario "scenario.xml"
check-file terrain "terrain.xml"
check-file items "items.xml"
check-file monsters "monsters.xml"

shopt -s nullglob

for sector in out/out*.xml; do
	check-file outdoor "$sector"
done

for town in towns/town*.xml; do
	check-file town "$town"
done

for speech in towns/talk*.xml; do
	check-file dialogue "$speech"
done

if [[ -d dialogs/ ]]; then
	for dlog in dialogs/*.xml; do
		check-file dialog "$dlog"
	done
fi

cd ..

if [[ $SCEN_PATH = scenario && ${1##*/} != scenario ]]; then
	rm -rf scenario
fi
