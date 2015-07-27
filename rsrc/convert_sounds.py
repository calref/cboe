#!/usr/bin/env python2

from __future__ import division
from math import ceil
import struct, sys

if len(sys.argv) < 1:
	print "No input files specified!"
	sys.exit(1)

arguments = sys.argv[1:]

def writeWAV(fname, fmt, chans, sampRate, bps, sndData):
	print 'Channels:', chans, 'Sample Rate:', sampRate, 'Bits per sample:', bps
	blockSz = (chans * bps) / 8
	avgBytesPerSec = blockSz * sampRate
	with open(fname, 'w') as output:
		output.write('RIFF')
		output.write(struct.pack('<L', 44 + len(sndData)))
		output.write('WAVEfmt ')
		output.write(struct.pack('<L2H2L2H', 16, fmt, chans, long(sampRate), avgBytesPerSec, blockSz, bps))
		output.write('data')
		output.write(struct.pack('<L', len(sndData)))
		output.write(sndData)

for fname in arguments:
	outfname = fname.replace('.sfil','.wav')
	print "Converting", fname, "->", outfname
	with open(fname, 'rb') as sndfile:
		fmt, = struct.unpack('<h', sndfile.read(2))
		if fmt == 1:
			# A type 1 sound
			numMods, = struct.unpack('<h', sndfile.read(2))
			while numMods > 0:
				modNum, modInit = struct.unpack('<HL', sndfile.read(6))
				# I don't think we need this data, so we'll discard it.
				numMods -= 1
			numCmds, = struct.unpack('<h', sndfile.read(2))
			while numCmds > 0:
				cmd, p1, p2 = struct.unpack('<Hhl4x', sndfile.read(12))
				numCmds -= 1
			sz, sampRate, loopS, loopE, fmt, baseFreq = struct.unpack('<LLLLBB', sndfile.read(18))
			sampRate /= 0x10000 # Because it's stored as a fixed-point number
			if fmt == 0:
				# A basic sound
				sndData = sndfile.read(sz)
				writeWAV(outfname, 1, 1, long(sampRate), 8, sndData)
			elif fmt == 0xfe:
				print "Error: Cannot convert compressed type 1 sounds!"
			elif fmt == 0xff:
				# An extended sound
				print "Error: Cannot convert extended type 1 sounds!"
			else:
				print "Error: Invalid type 1 sound format!"
		elif fmt == 2:
			# A type 2 sound
			refC, numCmds = struct.unpack('<hh', sndfile.read(4))
			while numCmds > 0:
				cmd, p1, p2 = struct.unpack('<Hhl4x', sndfile.read(12))
				numCmds -= 1
			sz, sampRate, loopS, loopE, fmt, baseFreq = struct.unpack('<LLLLBB', sndfile.read(18))
			sampRate /= 0x10000 # Because it's stored as a fixed-point number
			sndData = sndfile.read()
			writeWAV(outfname, 1, 1, long(sampRate), 8, sndData)
		else:
			# Invalid
			print "Error: This is not a type 1 or type 2 sound resource!"
			print "Format is #%i" % fmt

print "Finished!"
