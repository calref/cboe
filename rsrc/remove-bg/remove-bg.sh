#!/bin/bash

# EXPLANATION
#############
#
# This script was used to strip out the backgrounds from the living statue
# and crystal soul monster graphics. It takes as input three images:
#
# 1. back.png contains the backgrounds to be stripped out.
# 2. fore.png contains the original images.
# 3. mask.png contains black pixels where the original image should be assumed opaque.
#
# The actual magic that does this is contained in the two .fx scripts
# included in this directory.
#
# The script produces out.png containing the images with the background removed.
# Overall it works quite well; the regular crystal soul (normal and attack graphics)
# and the non-attacking living statue look pretty good with no additional
# manual adjustment. The other two crystal souls require a little noise removal,
# and the living statue attacks still require quite a bit of editing to get 
# a usable result. Also, though the crystal soul attack looked pretty good,
# I used GIMP's levels tool to make it look closer to the original.
# (Red down to 240, Green up to 80, Blue up to 80, Alpha down to 230)
#
# As might be obvious from the below commands, running this script
# requires that ImageMagick is installed.
#
# Note that this script is not at all generic. If you wanted to use it
# on other images you would at minimum have to edit the canvas size
# specified in the command.
#

convert \
	-size 113x148 canvas:white -alpha set \
	back.png fore.png -channel A -fx "@calc-alpha.fx" \
	back.png fore.png -channel RGB -fx "@apply-alpha.fx" \
	intermediate.png 2>| im_error.txt

convert intermediate.png fore.png mask.png -composite out.png

rm intermediate.png
