#!/bin/bash

shopt -s globstar

CSV=0
case $1 in 
    -csv|-CSV) 
        CSV=1
        echo "file;d;beta;px;py;mu1;m1;nrE;xMin;xMax;yMin;yMax;prec;absPrec;relPrec;remPrec;rays;inside;outside;error;norm;minRays;maxRays;time"
        shift
        ;;
    -*)
        echo "Unknown option!"
        exit
esac

INFO=$(which "info.awk")
if [ ! -x "$INFO" ]
then
    echo "The script $INFO is not executable."
    exit
fi

for d in $@/**/*out
do
    [ -d "$d" ] || continue
    pushd "$d" > /dev/null
    b=$(basename $d)
    dataFile="${b}.dat"
    if [ -r "$dataFile" ]
    then
        echo "Reading $dataFile..." >&2
    else
        echo "$dataFile is not readable!"
        continue
    fi

    "$INFO" -v "csv=$CSV" "$dataFile" || exit 
    echo ""
    popd > /dev/null
done

