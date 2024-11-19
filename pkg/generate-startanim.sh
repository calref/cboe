#!/bin/bash

magick convert +append pkg/credits/startanim-col-*.png pkg/credits/startanim-pt-2.png
magick convert -append pkg/credits/startanim-pt-*.png rsrc/graphics/startanim.png
# Clean up:
rm pkg/credits/startanim-col-*.png
rm pkg/credits/startanim-pt-2.png