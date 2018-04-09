#!/bin/bash

suff="pdf"

for d in *-out
do
    [ -d "$d" ] || continue

    pushd "$d"

    outFileBaseName="${d/%-out/}"
    outFile="${outFileBaseName}-out.dat"
    plotFile="${outFileBaseName}-plot.plt"
    pixelsFile="${outFileBaseName}-pixels.plt"
    confFile="${outFileBaseName}-conf.plt}"
    imgFile="${outFileBaseName}-rel.$suff"
    histFile="${outFileBaseName}-hist.$suff"

    if [ -f "$outFile" ]
    then
        echo "  Found ${outFile}."
    else
        echo "  No data file ${outFile}!"
        continue
    fi

    if [ "$outFile" -nt "$imgFile" ]  || [ "$confFile" -nt "$imgFile" ]
    then
        echo "Processing the file ${plotFile}." 
        gnuplot "$plotFile"
        if [ -r "${pixelsFile}" ]
        then
            echo "Processing the file ${pixelsFile}." 
            gnuplot "$pixelsFile"
        fi
    else
        echo "  File $imgFile is recent, skipping."
    fi

    if [ "$outFile" -nt "$histFile" ]
    then
        echo "Generating a histogram of relative errors..."
        draw_hist.jl "$outFile" 
    else
        echo "  File $histFile is recent, skipping."
    fi

    popd
        
done
