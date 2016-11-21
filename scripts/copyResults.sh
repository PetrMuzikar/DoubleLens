#!/bin/bash

shopt -s nullglob
shopt -s extglob

if [ "$#" -eq "0" ]
then
    echo "Please specify a name."
    exit 1
fi

DOUBLE_LENS_WORK_DIR="${DOUBLE_LENS_WORK_DIR:-${HOME}/Submit}"
echo "DOUBLE_LENS_WORK_DIR=${DOUBLE_LENS_WORK_DIR}"
if [ ! -d "$DOUBLE_LENS_WORK_DIR" ]
then
    echo "DOUBLE_LENS_WORK_DIR=${DOUBLE_LENS_WORK_DIR}\nPlease set a working directory."
    exit 1
fi

case "$1" in
    -l|--local)
    SELECT=1
    echo "Assuming computations were done on a regular machine."
    shift
    ;;
    -c|--cluster)
    SELECT=2
    echo "Assuming computations were done on the utf-cluster."
    shift
    ;;
    *)
    SELECT=2
    ;;
esac

for outFileBaseName in "$@"
do
    outDir="${DOUBLE_LENS_WORK_DIR}/outFiles/${outFileBaseName}-out"
    inDir="${DOUBLE_LENS_WORK_DIR}/inFiles/${outFileBaseName}-in"
    
    if [ -d "$outDir" ]
    then
        rm -rvf "$outDir"
    fi
    mkdir "$outDir"
    
    for d in "${outFileBaseName}"-+([[:digit:]])
    do
        case "$SELECT" in
            1)  # local
                f=(${d}/${d}-out\.dat)
                ;;
            2)  # cluster
                f=(${d}/+([[:digit:]])/${d}-out\.dat)
                ;;
        esac

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
