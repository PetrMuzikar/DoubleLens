#!/bin/bash

#chTerm() {
#    cat "$1" | sed 's/set term wxt/set term qt/g' | gnuplot
#}

for d in "$@"
do
    outFileBaseName="${d/%-out/}"
    outFile="${outFileBaseName}-out.dat"
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

    if [ -x "$(which julia)" ]
    then
        echo "Generating a histogram of relative errors..."
        draw_hist.jl "$outFile" 
    fi
    popd
done
