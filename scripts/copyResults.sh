#!/bin/bash

shopt -s nullglob
shopt -s extglob

for outFileBaseName in "$@"
do
    outDir="${HOME}/Submit/outFiles/${outFileBaseName}-out"
    inDir="${HOME}/Submit/outFiles/${outFileBaseName}-in"
    
    if [ -d "$outDir" ]
    then
    rm -rf "$outDir"
    fi
    mkdir "$outDir"
    
    for d in "${outFileBaseName}"-+([[:digit:]])
    do
        f=(${d}/+([[:digit:]])/${d}-out\.dat)
        # $f ... ${f[0]}
    	if [ -f "$f" ]
    	then
    	    echo "Directory $d, file ${f}."
    	    cp "$f" "$outDir"
    	else
    	    echo "File ${f} does not exist!"
    	    exit 1
    	fi
    done
done
