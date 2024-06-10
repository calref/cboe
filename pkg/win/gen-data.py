
from os.path import normpath as makepath
from glob import glob
import sys

root = makepath(sys.argv[1])

# To add directories to the list of sources to generate file lists from,
# simply edit this dictionary. The key is the directory path (relative to
# the build output directory), and the value is either a glob pattern to include,
# or an explicit list of files to include.

# All paths should be in UNIX format, since makepath() will be used to convert them.
# There should be no trailing slashes.

files = {
	'data/dialogs': '*.xml',
	'data/strings': '*.txt',
	'data/fonts': '*.ttf',
	'data/shaders': ['mask.frag', 'mask.vert'],
	'data/graphics': '*.png',
	'data/cursors': '*.gif',
	'data/sounds': '*.WAV',
}

for path, pattern in files.items():
	print('SetOutPath "' + makepath('$INSTDIR/' + path + '/') + '"')
	if type(pattern) == list:
		check_files = [root + '/' + path + '/' + x for x in pattern]
	else:
		check_files = glob(makepath(root + '/' + path + '/' + pattern))
	for fname in check_files:
		print('File "' + makepath(fname.replace(root, '${RELEASE_DIR}')) + '"')

