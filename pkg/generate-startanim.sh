#!/bin/bash

font_size=8
spacing=5
actual_line_size=$((font_size + spacing))

magick convert +append pkg/credits/startanim-col-*.png pkg/credits/startanim-pt-2.png
magick convert pkg/credits/startanim-pt-2.png -chop 0x$actual_line_size -gravity North pkg/credits/startanim-pt-2.png
magick convert -append pkg/credits/startanim-pt-*.png rsrc/graphics/startanim.png
# Clean up:
rm pkg/credits/startanim-col-*.png
rm pkg/credits/startanim-pt-2.png