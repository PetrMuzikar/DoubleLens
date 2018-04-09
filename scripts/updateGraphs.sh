#!/bin/bash

for d in $@
do
    pushd $d
    for b in b*
    do
        pushd $b
        all.sh
        popd
    done
    popd
done
