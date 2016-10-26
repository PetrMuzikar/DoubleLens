#!/bin/bash

shopt -s nullglob
shopt -s extglob

if [ "$#" -eq "0" ]
then
    echo "Please specify a directory containing the input files: *-in.dat and optionally *-conf.sh, *-conf.plt. The name of the directory must end with -in."
    exit 1
fi

DOUBLE_LENS_FIND_DOMAINS="$(which ${DOUBLE_LENS_FIND_DOMAINS:-FindDomains})"
if [ ! -x "$DOUBLE_LENS_FIND_DOMAINS" ]
then
    echo "The program $DOUBLE_LENS_FIND_DOMAINS is not executable. Please set the correct path."
    exit 1
fi

DOUBLE_LENS_EXEC="$(which ${DOUBLE_LENS_EXEC:-DoubleLens})"
if [ ! -x "$DOUBLE_LENS_EXEC" ]
then
    echo "The program $DOUBLE_LENS_EXEC is not executable. Please set the correct path."
    exit 1
fi

DOUBLE_LENS_DOMAINS="$(which ${DOUBLE_LENS_DOMAINS:-domains.awk})"
if [ ! -x "$DOUBLE_LENS_DOMAINS" ]
then
    echo "The script $DOUBLE_LENS_DOMAINS is not executable. Please set the correct path."
    exit 1
fi
    
# SOBOL_DIMS == 40 !!!

case "$1" in
    -l|--local)
    SELECT=1
    kMax=4
    shift
    ;;
    -c|--cluster)
    SELECT=2
    kMax=20
    shift
    ;;
    *)
    SELECT=2
    kMax=20
    ;;
esac

for d in "$@"
do
    outFileBaseName="${d/%-in/}"
    #echo "${outFileBaseName}"
    
    inFile="${outFileBaseName}-in.dat"
    confFile="${outFileBaseName}-conf.sh"
    outFile="${outFileBaseName}-out.dat"
    domainsFile="${outFileBaseName}-domains.dat"
    testFileBaseName="test"
    testFile="${testFileBaseName}.dat"
    testImagesFile="${outFileBaseName}-images.dat"
    dataFile="data.dat"
    plotMaps="maps.plt"
    plotFile="${outFileBaseName}-plot.plt"
    plotConfFile="${outFileBaseName}-conf.plt"
    inWorkFile="inWork.dat"
    
    if [ ! -d "$d" ]
    then
        mkdir "$d"
        mv "${outFileBaseName}"-*\.*(dat|sh|plt) "$d" || exit 1
    fi
    
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
    
    #plotConf=0
    #if [ -r "$plotConfFile" ]
    #then
    #    echo "Reading gnuplot config file ${plotConfFile}."
    #    cat "${plotConfFile}"
    #    plotConf=1
    #else
    #    echo "No gnuplot config file ${plotConfFile}, assuming default values..."
    #fi
    
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
        if [ $random -eq 0 ]
        then
            # core for Sobol
            printf " %s\n" $k >> $out
        fi
        cat $inFile >> $out
        echo "${pixels}" >> $out
        (( k++ ))
    done < "$domainsFile"
    
    rm -rvf "${testFile}" "${dataFile}" "err.dat"
    
    popd
done
