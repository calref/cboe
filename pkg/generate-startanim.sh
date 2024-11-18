#!/bin/bash

text="$(cat)"
line_count=$(echo "$text" | wc -l)

font_size=8
spacing=5
actual_line_size=$((font_size + spacing))
first_line_offset=$((font_size + 2))
height=$((actual_line_size * line_count))

magick -size 280x$height xc:transparent \
    -stroke black -pointsize $font_size -interline-spacing $spacing \
    -font 'pkg/credits/font-8.bdf' -annotate +7+$first_line_offset "$text" \
    pkg/credits/startanim-pt-2.png

magick convert -append pkg/credits/startanim-pt-*.png rsrc/graphics/startanim.png