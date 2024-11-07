#!/bin/bash

names=$(grep ^\* DONORS.md)
name_count=$(echo "$names" | wc -l)

# For clear text, render higher point size at higher density
font_size=8
spacing=0
actual_line_size=$((font_size + spacing + 7))
first_line_offset=$((font_size + 2))
height=$((actual_line_size * name_count))

# https://www.dafont.com/pix-chicago.font

# TODO calculate line height
magick convert -size 280x$height xc:transparent \
    -stroke black -pointsize $font_size -interline-spacing $spacing \
    -kerning 1 -font pix-Chicago -annotate +0+$first_line_offset "$names" \
    startanim2.png 