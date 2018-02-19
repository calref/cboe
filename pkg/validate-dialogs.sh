#!/bin/bash
# Usage: validate-dialogs.sh path-to-rsrc

function filter() {
	grep -v 'validates' -
	return 0
}

cd "$1/dialogs/"
set -o pipefail
xmllint --nonet --noout --schema "$1/schemas/dialog.xsd" "$1/dialogs/"*.xml 2>&1 | sed 's/%20/ /g' | filter

