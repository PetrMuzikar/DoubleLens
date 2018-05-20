#!/bin/bash

pushd() {
    command pushd "$@" > /dev/null
}

popd() {
    command popd "$@" > /dev/null
}

inFiles="$(pwd)/../inFiles"
#echo $inFiles

for d in d*
do
    pushd "$d"
    for b in b*
    do
        pushd "$b"
        for s in *
        do
            [ -d "$s" ] || continue
            pushd "$s"
            bn="${s/-out/}"
            confFile="${bn}-conf.plt"
            inConfFile="${inFiles}/${bn}-in/${confFile}"
            if [ -f "$confFile" ] 
            then
                cp -uv "$confFile" "$inConfFile"
            fi
            popd
        done
        popd
    done
    popd
done
