#!/bin/bash

shopt -s nullglob
shopt -s extglob

for d in "$@"
do
    [ -d "$d" ] || continue
    outFileBaseName="${d/%-out/}"
    pushd "$d"
    files=("${outFileBaseName}"-+([[:digit:]])-out\.dat)
    rm -vf "${files[@]}"
    popd
done

rm -vf err.dat
