#!/bin/bash
build="$1"
file="$2"
if [ -f "$file" ] ; then
    rm "$file"
fi
echo "$build" >> "$file"
