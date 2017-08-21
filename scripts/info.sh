#!/bin/bash

INFO=$(which "info.awk")
if [ ! -x "$INFO" ]
then
    echo "The script $INFO is not executable."
    exit
fi

for d in $@
do
    [ -d "$d" ] || continue
    pushd "$d" > /dev/null
    dataFile="${d}.dat"
    if [ ! -r "$dataFile" ]
    then
        echo "$dataFile is not readable!"
        continue
    fi
   
    "$INFO" "$dataFile" || exit 
    echo ""
    popd > /dev/null
done

