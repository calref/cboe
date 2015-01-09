
from os.path import relpath as makepath
from glob import glob

root = makepath("../Release")

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
	'Scenario Editor/graphics.exd/mac': '*.png',
	'Scenario Editor/graphics.exd/mac/cursors': '*.gif',
	'Scenario Editor/sounds.exa': '*.WAV',
}

for path, pattern in files.items():
	print 'SetOutPath', '"' + makepath("$INSTDIR/" + path + '/') + '"'
	if type(pattern) == list:
		check_files = [root + '/' + path + '/' + x for x in pattern]
	else:
		check_files = glob(root + '/' + path + '/' + pattern)
	for fname in check_files:
		print 'File', '"' + makepath(fname) + '"'

