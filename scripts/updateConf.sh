#!/bin/bash

inFiles="$(pwd)/../inFiles"
echo $inFiles

for d in d*
do
    pushd "$d"
    for b in b*
    do
        pushd "$b"
        for s in *
        do
            pushd "$s"
            bn="${s/-out/}"
            confFile="${s}-conf.plt"
            inConfFile="${inFiles}/${bn}-in/${confFile}"
            cp -u "$confFile" "$inConfFile"
            popd
        done
        popd
    done
    popd
done
