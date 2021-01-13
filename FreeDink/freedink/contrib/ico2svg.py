#!/usr/bin/python3
# Create pixel-art SVG
# 
# Copyright (C) 2019 Sylvain Beucler
# 
# This file is part of GNU FreeDink
# 
# GNU FreeDink is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# GNU FreeDink is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see
# <http://www.gnu.org/licenses/>.

# Note: freedink.svg however was created manually due to limitations
# of SVG renderers when processing side-by-side squares (grid effect);
# work-around: add 0.3 to squares size, but the manual svg was smaller
# and more accurate anyway

# Incidentally, to round values:
# perl -w -p -e 's/( |,)(-?\d+\.\d+)/sprintf(" %.0f",$2)/ge' file1.svg > file2.svg

origsize=32
size=32
print("""<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" width="{size}px" height="{size}px" viewBox="0 0 {size} {size}" xml:space="preserve">""".format(size=size))

import PIL.Image
import sys
i = PIL.Image.open(sys.argv[1])
scale = size/origsize
for x in range(0,32):
    for y in range(0,32):
        (r,g,b,a) = i.getpixel((x,y))
        if a == 0:
            continue
        hexcol = '#{:02x}{:02x}{:02x}'.format(r,g,b)
        print("""<rect style="fill:{hexcol};stroke:none;fill-opacity:1" width="{scale}" height="{scale}" x="{x}" y="{y}" />""".format(hexcol=hexcol, x=x*scale, y=y*scale, scale=scale+.3))

print("""</svg>""")
