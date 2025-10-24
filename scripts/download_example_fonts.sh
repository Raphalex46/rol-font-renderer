#! /bin/bash

# A small script to download some example fonts. This script doesn't check if
# the fonts already exists so it will just overwrite every existing font in the
# examples/common/fonts directory.

fonts_dir=$(realpath ./examples/common/fonts)
mkdir -p $fonts_dir

# Download BDF fonts
mkdir -p $fonts_dir/bdf
pushd $fonts_dir/bdf
wget https://unifoundry.com/pub/unifont/unifont-17.0.02/font-builds/unifont-17.0.02.bdf.gz -O unifont.bdf.gz
gunzip -f unifont.bdf.gz
popd

# Download PCF fonts
mkdir -p $fonts_dir/pcf
pushd $fonts_dir/pcf
wget https://unifoundry.com/pub/unifont/unifont-17.0.02/font-builds/unifont-17.0.02.pcf.gz -O unifont.pcf.gz
gunzip -f unifont.pcf.gz
popd
