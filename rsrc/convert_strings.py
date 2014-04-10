#!/usr/bin/env python2

import struct, sys

if len(sys.argv) < 1:
	print "No input files specified!"
	sys.exit(1)

arguments = sys.argv[1:]

for fname in arguments:
	outfname = fname.replace('.str#','.txt')
	print "Converting", fname, "->", outfname
	with open(fname, 'rb') as strlist:
		numStrings = struct.unpack('>H', strlist.read(2))[0]
		allStrings = []
		while numStrings > 0:
			thisLen = struct.unpack('>B', strlist.read(1))[0]
			allStrings.append(struct.unpack(`thisLen` + 's', strlist.read(thisLen))[0])
			numStrings = numStrings - 1
	with open(outfname, 'w') as output:
		for nextString in allStrings:
			print >> output, nextString.replace('\r','|')

print "Finished!"