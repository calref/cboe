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
# To reproduce the crystal soul and living statue work, use the following params:
#   ./remove-bg.sh 113x148 back.png fore.png mask.png out.png
#

if [ $# != 5 ]; then
	echo 'Usage:'
	echo "  $0 <size> <back> <fore> <mask> <out>"
	echo
	echo '     <size>   The size of the image, eg 28x36'
	echo '     <back>   The path of the background image'
	echo '     <fore>   The path of the foreground image'
	echo '     <mask>   The path of the masking image'
	echo '     <out>    The path for the output image'
	echo
	echo 'All parameters are required.'
	echo 'If it fails, check im_error.txt for ImageMagick errors.'
	exit
fi

if [ -z `which convert` ]; then
	echo 'Error! You need to install ImageMagick before you'
	echo 'can use this script.'
	exit
fi

if [ ! -e "$2" -o ! -e "$3" -o ! -e "$4" ]; then
	echo 'Error! Could not find one of the input files!'
	echo 'Make sure you spelled them all correctly.'
	exit
fi

convert \
	-size $1 canvas:white -alpha set \
	"$2" "$3" -channel A -fx "@calc-alpha.fx" \
	"$2" "$3" -channel RGB -fx "@apply-alpha.fx" \
	intermediate.png 2>| im_error.txt

convert intermediate.png "$3" "$4" -composite "$5"

rm intermediate.png
