#!/bin/bash

shopt -s nullglob
shopt -s extglob

if [ "$#" -eq "0" ]
then
    echo "Please specify a directory containing the input files: *-in.dat and optionally *-conf.sh, *-conf.plt. The name of the directory must end with -in."
    exit 1
fi

DOUBLE_LENS_FIND_DOMAINS="$(which ${DOUBLE_LENS_FIND_DOMAINS:-FindDomains})"
echo "DOUBLE_LENS_FIND_DOMAINS=$DOUBLE_LENS_FIND_DOMAINS"
if [ ! -x "$DOUBLE_LENS_FIND_DOMAINS" ]
then
    echo "The program $DOUBLE_LENS_FIND_DOMAINS is not executable. Please set the correct path."
    exit 1
fi

DOUBLE_LENS_EXEC="$(which ${DOUBLE_LENS_EXEC:-DoubleLensSobolSimple})"
echo "DOUBLE_LENS_EXEC=$DOUBLE_LENS_EXEC"
if [ ! -x "$DOUBLE_LENS_EXEC" ]
then
    echo "The program $DOUBLE_LENS_EXEC is not executable. Please set the correct path."
    exit 1
fi

echo "DOUBLE_LENS_SCRIPTS=$DOUBLE_LENS_SCRIPTS"

DOUBLE_LENS_DOMAINS="$DOUBLE_LENS_SCRIPTS/domains.awk"
echo "DOUBLE_LENS_DOMAINS=$DOUBLE_LENS_DOMAINS"
if [ ! -x "$DOUBLE_LENS_DOMAINS" ]
then
    echo "The script $DOUBLE_LENS_DOMAINS is not executable. Please set the correct path."
    exit 1
fi

# SOBOL_DIMS == 40 !!!

case "$1" in
    utf-old|utf-cluster)
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

echo "Machine: ${SELECT}, max. processes: ${kMax}."

for d in "$@"
do
    [ -d "$d" ] || continue

    outFileBaseName="${d/%-in/}"
    
    inFile="${outFileBaseName}-in.dat"
    confFile="${outFileBaseName}-conf.sh"
    outFile="${outFileBaseName}-out.dat"
    domainsFile="${outFileBaseName}-domains.dat"
    testFile="${outFileBaseName}-test.dat"
    testImagesFile="${outFileBaseName}-images.dat"
    dataFile="${outFileBaseName}-data.dat"
    plotMaps="maps.plt"
    plotFile="${outFileBaseName}-plot.plt"
    plotConfFile="${outFileBaseName}-conf.plt"
    
    #if [ ! -d "$d" ]
    #then
    #    mkdir "$d"
    #    mv "${outFileBaseName}"-*\.*(dat|sh|plt) "$d" || exit 1
    #fi
    
    pushd "$d"
    
    if [ -r "$inFile" ]
    then
        echo "Reading input file ${inFile}."
        cat "${inFile}"
    else
        echo "The input file ${inFile} is not readable!"
        exit 1
    fi
    
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
    tol=${tol:-"0.1"}
    images=${images:-10000}
    
    clean=${clean:-1}

    echo "outFileBaseName: ${outFileBaseName}"
    echo "random: ${random}"
    echo "integ: ${integ}"
    echo "radius: ${radius}"
    echo "rays: ${rays}"
    echo "raysGroup: ${raysGroup}"
    echo "tol: ${tol}"
    echo "images: ${images}"
    echo "pixels: ${pixels}"
    echo "clean: ${clean}"
    
    echo "Computing images of source plane..."
    echo " $images" > $dataFile
    cat $inFile >> $dataFile
    ${DOUBLE_LENS_EXEC} i ${testFile} ${dataFile} || exit 1
    
    echo "Separating images into groups..." 
    cat ${testFile} | awk '/^#/{next;} {print $3, $4;}' | ${DOUBLE_LENS_FIND_DOMAINS} $radius $tol > "$testImagesFile"
    
    echo "Preparing input files..."
    rm -vf "${outFileBaseName}"-+([[:digit:]])\.dat
    S=$(${DOUBLE_LENS_DOMAINS} ${testImagesFile})
    ${DOUBLE_LENS_DOMAINS} -v "Sread=$S" -v "n=$rays" -v "nMax=$raysGroup" "$testImagesFile" > "$domainsFile" 
    raysReal=$(awk '/^$|^#/{next;} {n += $NF;} END{print n;}' "$domainsFile")
    echo "# Ssum= ${S}" >> "$testImagesFile"
    echo "# rays= ${raysReal}" >> "$testImagesFile"
    if [ "$rays" -ne "$raysReal" ]
    then
        echo "rays = $rays"
        echo "raysReal = $raysReal"
    fi
    
    k=0
    startOfSequence=0
    while read -r line || [[ -n "$line" ]]
    do
        if [ $k -ge $kMax ]
        then
            echo "Maximum of $kMax input files reached!"
	    echo "Please increase the value of raysGroup."
	    echo "raysGroup = ${raysGroup}"
            exit 1
        fi
        out="${outFileBaseName}-$k.dat"
        echo "$out"
        echo "$line" > "$out"
        if [ $random -ne 1 ]
        then
            # start of Sobol or Halton sequence
            printf " %s\n" $startOfSequence >> $out
            add=$(echo "$line" | awk '{print $NF}')
            (( startOfSequence += add ))
        else
            echo " $k " >> "$out"
        fi
        cat $inFile >> $out
        echo "${pixels}" >> $out
        (( k++ ))
    done < "$domainsFile"
    
    if [ $clean -eq 1 ]
    then
        rm -rvf "${testFile}" "${dataFile}" "err.dat"
    fi

    popd
done
