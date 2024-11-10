#!/bin/bash

text="$(cat)"
line_count=$(echo "$text" | wc -l)

font_size=8
spacing=0
actual_line_size=$((font_size + spacing + 7))
first_line_offset=$((font_size + 2))
height=$((actual_line_size * line_count))

# https://www.dafont.com/pix-chicago.font

magick -size 280x$height xc:transparent \
    -stroke black -pointsize $font_size -interline-spacing $spacing \
    -kerning 1 -font pix-Chicago -annotate +6+$first_line_offset "$text" \
    pkg/credits/startanim2.png

magick convert -append pkg/credits/startanim*.png rsrc/graphics/startanim.png