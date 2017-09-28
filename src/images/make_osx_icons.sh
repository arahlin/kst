#!/bin/sh

[ ! -e kst_hires.png ] && convert -size 2048x2048 -background none svg_icons/kst.svg kst_hires.png

input=kst_hires.png
iconset=kst.iconset
output=kst.icns

# create iconset
rm -rf ${iconset} && mkdir ${iconset}
for sz in 16 32 64 128 256 512; do
    icsfile=${iconset}/icon_${sz}x${sz}
    sz2=$((sz * 2))
    sips --resampleHeightWidth ${sz} ${sz} ${input} --out ${icsfile}.png &> /dev/null
    sips --resampleHeightWidth ${sz2} ${sz2} ${input} --out ${icsfile}@2x.png &> /dev/null
done

# convert to icns file
iconutil --convert icns ${iconset} --output ${output}
