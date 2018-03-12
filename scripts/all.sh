#!/bin/bash

suff="${1:-pdf}"

for d in *-out
do
    [ -d "$d" ] || continue
    echo "Folder: $d"
    dataFile="${d}/${d}.dat"
    confFile="${d}/${d/%-out/-conf.plt}"
    imgFile="${d}/${d}-rel.$suff"
    if [ -f "$dataFile" ]
    then
        echo "  Found $dataFile."
    else
        echo "  No data file $dataFile!"
        continue
    fi
    if [ "$dataFile" -nt "$imgFile" ]  || [ "$confFile" -nt "$imgFile" ]
    then
        graph.sh "$d"
    else
        echo "  File $imgFile is recent, skipping."
    fi
done
