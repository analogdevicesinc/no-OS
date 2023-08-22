#!/bin/bash

url=$(curl -s https://sourceforge.net/projects/ezwinports/files/ | grep -o -E 'href="([^"#]+/make-[^"#]+-without-guile-w32-bin\.zip/download)"' | sed -e 's/href="//' -e 's/"$//' | head -n 1)
filename=$(echo "$url" | sed -E 's|.*/(make-[0-9]+\.[0-9]+\.[0-9]+-without-guile-w32-bin\.zip)/download|\1|')
# Download and extract directly to /mingw64/bin
curl -L "$url" -o "/tmp/$filename"
unzip -j -o "/tmp/$filename" -d /mingw64/bin
rm "/tmp/$filename"

echo "make is now available in /mingw64/bin."
