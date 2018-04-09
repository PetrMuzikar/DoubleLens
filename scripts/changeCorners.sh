#!/bin/bash

for d in $@
do
    pushd $d
    for b in b*
    do
        pushd $b
        for out in *-out
        do
            echo "Modifying $out..."
            changeCorners.awk "$out/${out}.dat" > "$out/w" \
                && mv "$out/w" "$out/${out}.dat"
        done
        popd
    done
    popd
done

