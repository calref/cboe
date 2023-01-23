
while getopts "u:h:p:" OPT; do
	case $OPT in
		u) UPLOAD_USER=$OPTARG;;
		h) UPLOAD_HOST=$OPTARG;;
		p) UPLOAD_PATH=$OPTARG;;
		\?) exit 1;;
	esac
done

if [ -z "$UPLOAD_USER" ]; then
	echo "Missing user -u"
	exit 1
fi

if [ -z "$UPLOAD_HOST" ]; then
	echo "Missing host -h"
	exit 1
fi

if [ -z "$UPLOAD_PATH" ]; then
	echo "Missing path -p"
	exit 1
fi

echo "Uploading documentation files to $UPLOAD_USER@$UPLOAD_HOST"

rsync -rptv \
	--exclude-from=doc/upload_exclude.txt \
	--delete --delete-excluded \
	doc/ $UPLOAD_USER@$UPLOAD_HOST:$UPLOAD_PATH
