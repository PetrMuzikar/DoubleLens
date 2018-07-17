#!/bin/bash 

for inDir in $@
do
    pushd "$inDir" > /dev/null
    f="$(basename ${inDir})"
    inFile="${f/%in/images.dat}"
    echo "$inFile"
    if [ -r "$inFile" ]
    then 
        intersection.awk "$inFile"
    fi
    popd > /dev/null
done
