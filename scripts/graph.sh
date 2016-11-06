#!/bin/bash

#chTerm() {
#    cat "$1" | sed 's/set term wxt/set term qt/g' | gnuplot
#}

for d in "$@"
do
    outFileBaseName="${d/%-out/}"
    plotFile="${outFileBaseName}-plot.plt"
    pixelsFile="${outFileBaseName}-pixels.plt"
    pushd "$d"
    echo "Processing the file ${plotFile}." 
    gnuplot "$plotFile"
    if [ -r "${pixelsFile}" ]
    then
        echo "Processing the file ${pixelsFile}." 
        gnuplot "$pixelsFile"
    fi
    popd
done
