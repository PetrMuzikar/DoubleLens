#!/bin/bash

INFO="./info.awk"
if [ ! -x "$INFO" ]
then
    echo "The script $INFO is not executable."
    exit
fi

for d in $@
do
    [ -d "$d" ] || continue
    #echo "Folder: $d"
    dataFile="${d}/${d}.dat"
    if [ ! -r "$dataFile" ]
    then
        echo "$dataFile is not readable!"
        continue
    fi
   
    "$INFO" "$dataFile" || exit 
done

