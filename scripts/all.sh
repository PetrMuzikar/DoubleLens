#!/bin/bash

for d in *-out
do
    [ -d "$d" ] || continue
    echo "Folder: $d"
    dataFile="${d}/${d}.dat"
    epsFile="${d}/${d}-rel.eps"
    if [ -f "$dataFile" ]
    then
        echo "  Found $dataFile."
    else
        echo "  No data file $dataFile!"
        continue
    fi
    if [ "$dataFile" -nt "$epsFile" ]
    then
        graph.sh "$d"
    else
        echo "  File $epsFile is recent, skipping."
    fi
done
