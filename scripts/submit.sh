#!/bin/bash

shopt -s nullglob
shopt -s extglob

if [ "$#" -eq "0" ]
then
    echo "Please specify a directory containing the input files generated by the setup.sh script. The name of the directory must end with -in."
    exit 1
fi

echo "DOUBLE_LENS=$DOUBLE_LENS"
if [ ! -d "$DOUBLE_LENS" ]
then
    echo "DOUBLE_LENS=${DOUBLE_LENS}\nPlease set the main project directory."
    exit 1
fi

DOUBLE_LENS_WORK_DIR="${DOUBLE_LENS_WORK_DIR:-${HOME}/Submit}"
echo "DOUBLE_LENS_WORK_DIR=${DOUBLE_LENS_WORK_DIR}"
if [ ! -d "$DOUBLE_LENS_WORK_DIR" ]
then
    echo "DOUBLE_LENS_WORK_DIR=${DOUBLE_LENS_WORK_DIR}\nPlease set a working directory."
    exit 1
fi

echo "DOUBLE_LENS_SCRIPTS=$DOUBLE_LENS_SCRIPTS"

DOUBLE_LENS_SHOOTING="$DOUBLE_LENS_SCRIPTS/shooting.sh"
echo "DOUBLE_LENS_SHOOTING=$DOUBLE_LENS_SHOOTING"
if [ ! -x "$DOUBLE_LENS_SHOOTING" ]
then
    echo "DOUBLE_LENS_SHOOTING=${DOUBLE_LENS_SHOOTING}\nPlease set the correct path to the script shooting.sh."
fi

case "$1" in
    utf)
        SELECT="$1"
        kMax=46
        shift
        ;;
    cronus)
        SELECT="$1"
        kMax=20
        shift
        ;;
    *)
        SELECT="generic"
        kMax=4
esac

for d in "$@"
do

    outFileBaseName="${d/%-in/}"
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
        0) executable="${DOUBLE_LENS}/bin/Release/DoubleLensSobolSimple" ;;
        1) executable="${DOUBLE_LENS}/bin/Release/DoubleLensRandomSimple" ;;
        2) executable="${DOUBLE_LENS}/bin/Release/DoubleLensSobolInteg" ;;
        3) executable="${DOUBLE_LENS}/bin/Release/DoubleLensRandomInteg" ;;
        *) executable="none" ;;
    esac
    
    if [ -x "$executable" ]
    then
        echo "Using $executable"
    else
        echo "$executable is not executable!"
        exit 1
    fi
    
    rm -rf "${DOUBLE_LENS_WORK_DIR}/${outFileBaseName}"-+([[:digit:]])

    k=0
    for f in "${outFileBaseName}"-+([[:digit:]])\.dat
    do
        dirName="${DOUBLE_LENS_WORK_DIR}/${f/%.dat/}"
        echo "$dirName"
        mkdir "$dirName"
        outFile="${f/%.dat/-out.dat}"
        #errFile="${f/%.dat/-err.dat}"
        cp "$f" "${dirName}"
        pushd "$dirName"
        case "$SELECT" in
            utf)
                qsub -cwd -l h_vmem=500m,h_fsize=500m -v EXEC="${executable}" -v SELECT="${SELECT}" "${DOUBLE_LENS_SHOOTING}"
                ;;
            cronus)
                qsub -l walltime=120:00:00 -v EXEC="${executable}" -v SELECT="${SELECT}" "${DOUBLE_LENS_SHOOTING}"
                ;;
            generic)
                nohup "${executable}" s "${outFile}" "$f" &
                ;;
        esac
        popd
        (( k++ ))
    done
done

case "$SELECT" in
    utf)
        qstat -f
        ;;
    cronus)
        qstat -u "pmuzikar"
        ;;
    generic)
        echo "Running jobs:"
        ps -u $USER -o pid,cmd | grep "$(basename ${executable})"
        ;;
esac

