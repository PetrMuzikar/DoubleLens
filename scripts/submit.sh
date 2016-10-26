#!/bin/bash

shopt -s nullglob
shopt -s extglob

prefix="/home/pmuzikar/Lensing/DoubleLens"

#d="$1"
for d in "$@"
do

    outFileBaseName=${d/%-in/}
    confFile="${outFileBaseName}-conf.sh"

    [ -d "$d" ] || continue
    
    pushd "$d"
    
    if [ -r "$confFile" ]
    then
        echo "Reading config file ${confFile}."
        source "$confFile"
        cat "$confFile"
    else
        echo "No config file ${confFile}, assuming default values..."
    fi
    
    random=${random:-0}
    integ=${integ:-0}
    radius=${radius:-1}
    rays=${rays:-100000000}
    raysGroup=${raysGroup:-50000000}
    tol=${tol:-"0.05"}
    images=${images:-10000}
    
    echo "outFileBaseName: ${outFileBaseName}"
    echo "random: ${random}"
    echo "integ: ${integ}"
    echo "radius: ${radius}"
    echo "rays: ${rays}"
    echo "raysGroup: ${raysGroup}"
    echo "tol: ${tol}"
    echo "images: ${images}"
    echo "pixels: ${pixels}"
    
    choice=$(( random + 2 * integ ))
    case $choice in
        0) executable="${prefix}/bin/Release/DoubleLensSobolSimple" ;;
        1) executable="${prefix}/bin/Release/DoubleLensRandomSimple" ;;
        2) executable="${prefix}/bin/Release/DoubleLensSobolInteg" ;;
        3) executable="${prefix}/bin/Release/DoubleLensRandomInteg" ;;
        *) executable="none" ;;
    esac
    
    if [ -x "$executable" ]
    then
        echo "Using $executable"
    else
        echo "$executable is not executable!"
        exit 2
    fi
    
    rm -rf "${HOME}/Submit/${outFileBaseName}"-+([[:digit:]])
    #ls -d "${HOME}/Submit/${outFileBaseName}"-+([[:digit:]])

    k=0
    for f in "${outFileBaseName}"-+([[:digit:]])\.dat
    do
        dirName="${HOME}/Submit/${f/%.dat/}"
        echo "$dirName"
        mkdir "$dirName"
        cp "$f" "${dirName}"
        cp "${HOME}/bin/shooting.sh" "${dirName}"
        pushd "$dirName"
        qsub -cwd -l h_vmem=500m,h_fsize=500m -v EXEC="${executable}" shooting.sh
         popd
        (( k++ ))
    done
done

qstat -f
