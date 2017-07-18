#!/bin/bash

shopt -s nullglob
shopt -s extglob

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
echo "GNUPLOT_LIB=$GNUPLOT_LIB"

DOUBLE_LENS_DOMAINS="$DOUBLE_LENS_SCRIPTS/domains.awk"
echo "DOUBLE_LENS_DOMAINS=$DOUBLE_LENS_DOMAINS"
if [ ! -x "$DOUBLE_LENS_DOMAINS" ]
then
    echo "The script $DOUBLE_LENS_DOMAINS is not executable. Please set the correct path."
    exit 1
fi

DOUBLE_LENS_DRAW_DOMAINS="$DOUBLE_LENS_SCRIPTS/drawDomains.awk"
echo "DOUBLE_LENS_DRAW_DOMAINS=$DOUBLE_LENS_DRAW_DOMAINS"
if [ ! -x "$DOUBLE_LENS_DRAW_DOMAINS" ]
then
    echo "The script $DOUBLE_LENS_DRAW_DOMAINS is not executable. Please set the correct path."
    exit 1
fi

DOUBLE_LENS_INPUT="$DOUBLE_LENS_SCRIPTS/input.awk"
echo "DOUBLE_LENS_INPUT=$DOUBLE_LENS_INPUT"
if [ ! -x "$DOUBLE_LENS_INPUT" ]
then
    echo "The script $DOUBLE_LENS_INPUT is not executable. Please set the correct path."
    exit 1
fi

DOUBLE_LENS_HIST="$DOUBLE_LENS_SCRIPTS/draw_hist.jl"
echo "DOUBLE_LENS_HIST=$DOUBLE_LENS_HIST"
if [ ! -x "$DOUBLE_LENS_HIST" ]
then
    echo "The script $DOUBLE_LENS_HIST is not executable. Please set the correct path."
    exit 1
fi

DOUBLE_LENS_PLOT="${DOUBLE_LENS_SCRIPTS}/maps.plt"
echo "DOUBLE_LENS_PLOT=$DOUBLE_LENS_PLOT"
if [ ! -r "$DOUBLE_LENS_PLOT" ]
then
    echo "The gnuplot script DOUBLE_LENS_PLOT=$DOUBLE_LENS_PLOT is not readable. Please set the correct path."
    exit 1
fi

if [ -x "$(which gnuplot)" ]
then
    recentGnuplot="$(gnuplot -V | awk '{print ($2 >= 4.6);}')"
else
    recentGnuplot="0"
fi

for d in "$@"
do
    outFileBaseName="${d/%-out/}"
    
    inPrefix="${DOUBLE_LENS_WORK_DIR}/inFiles/${outFileBaseName}-in"
    
    inFile="${inPrefix}/${outFileBaseName}-in.dat"
    confFile="${inPrefix}/${outFileBaseName}-conf.sh"
    domainsFile="${inPrefix}/${outFileBaseName}-domains.dat"
    testImagesFile="${inPrefix}/${outFileBaseName}-images.dat"
    plotConfFile="${inPrefix}/${outFileBaseName}-conf.plt"
    
    outFile="${d}/${outFileBaseName}-out.dat"
    plotFile="${d}/${outFileBaseName}-plot.plt"
    pixelsFile="${d}/${outFileBaseName}-pixels.dat"
    pixelsPlotFile="${d}/${outFileBaseName}-pixels.plt"
    pixelsPlotEps="${d}/${outFileBaseName}-pixels.eps"
    inWorkFile="${d}/${outFileBaseName}-inWork.dat"
    numRaysFile="${d}/${outFileBaseName}-numRays.dat"
    #testFileBaseName="test"
    #testFile="${testFileBaseName}.dat"
    #dataFile="data.dat"
    
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
    
    plotConf=0
    if [ -r "$plotConfFile" ]
    then
        echo "Reading gnuplot config file ${plotConfFile}."
        cat "${plotConfFile}"
        cp "${plotConfFile}" "$d"
        plotConf=1
    else
        echo "No gnuplot config file ${plotConfFile}, assuming default values..."
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
    
    #prefix="${HOME}/Lensing/DoubleLens"
    choice=$(( integ + 2 * random ))
    case $choice in
        0) executable="${DOUBLE_LENS}/bin/DoubleLensSobolSimple" ;;
        1) executable="${DOUBLE_LENS}/bin/DoubleLensSobolInteg" ;;
        2) executable="${DOUBLE_LENS}/bin/DoubleLensRandomSimple" ;;
        3) executable="${DOUBLE_LENS}/bin/DoubleLensRandomInteg" ;;
        4) executable="${DOUBLE_LENS}/bin/DoubleLensHaltonSimple" ;;
        5) executable="${DOUBLE_LENS}/bin/DoubleLensHaltonInteg" ;;
        *) executable="none" ;;
    esac
    
    if [ -x $executable ]
    then
        echo "Using $executable"
    else
        echo "$executable is not executable!"
        exit 1
    fi
    
    echo "Combining results..."
    echo "" > "$numRaysFile"
    files=("${d}/${outFileBaseName}"-+([[:digit:]])-out.dat)
    for f in ${files[@]}
    do
        echo "$f"
        tac "$f" | awk '/error=/{e = $3; next;} /outside=/{o = $3; next;} /inside=/{i = $3; next;} /rays=/{r = $3; exit;} END{ printf("%15d %15d %15d %15d\n", r, i, o, e);}' >> "$numRaysFile"
    done
    rr=$(awk '/^$|^#/{next;} {r += $1; e += $4;} END{print r-e;}' "$numRaysFile")
    tail -n 3 "$testImagesFile" | awk 'BEGIN{s=0} /^# Ssum=/{s = $3} END{printf("r %.12e 0 1 0 %.12e ", s, s)}' > $inWorkFile 
    echo "$rr " >> "$inWorkFile"
    # where to start a Sobol or Halton sequence, random seed
    echo "0" >> "$inWorkFile"
    cat "$inFile" >> "$inWorkFile"
    if [ "$pixels" != "" ]
    then
        echo "$pixels" >> "$inWorkFile"
    fi
    "$executable" c "$outFile" "$inWorkFile" "${files[@]}"
    for f in ${files[@]}
    do
        cat "$f" | awk '/^# Time:/{ printf("# computation time: %.2e s = %.2f h\n", $3, $3 / 3600)} {next;}' >> "$outFile"
    done
    cat "$outFile" | awk 'BEGIN{t=0} /^# computation/{ t += $4 } {next} END{ printf("# total time: %e s = %.2f h\n", t, t / 3600) }' >> "$outFile"
    
    if [ "${pixels}" != "" ]
    then
        echo "Collecting informations about rays recorded in selected pixels..."
        cat "${files[0]}" | grep "^#p" | tee "$pixelsFile"
        grep -h "^#rtp" "${files[@]}" | sed 's/^#rtp//g' | sort -k1n,2 | awk 'BEGIN{n=0;} /^$|^#/{next} {n++; if (n == 1) {i = $1; j = $2; print; next;} if ((i != $1) || (j != $2)) { i=$1; j=$2; print "\n"; print $0; } else {print $0} }' >> "$pixelsFile"
    fi
    
    echo "Creating gnuplot file(s)..."
    echo "plotConf = ${plotConf}" > "$plotFile"
    echo "plotConfFile = \"$(basename ${plotConfFile})\"" >> "$plotFile"
    #echo "maps = system(\"which ${DOUBLE_LENS_PLOT}\")" >> "$plotFile"
    echo -n "call \"$(basename $DOUBLE_LENS_PLOT)\" \"$(basename ${d}/${outFileBaseName}-out)\" " >> "$plotFile"
    head "${outFile}" | grep "##" | "${DOUBLE_LENS_INPUT}" -v "todo=0" -v "random=${random}" >> "$plotFile"
    echo "File ${plotFile}:"
    cat $plotFile

    if [ "${pixels}" != "" ]
    then
        printf "unset key\n@EPS\n" > "$pixelsPlotFile"
        echo "set out \"$(basename $pixelsPlotEps)\"" >> "$pixelsPlotFile"
        echo "p \"$(basename ${pixelsFile})\" u 5:6:(column(-2)+1) lc variable" >> "$pixelsPlotFile"
        printf "unset out\n@DEFAULT\nreplot\n" >> "$pixelsPlotFile"
        echo "File ${pixelsPlotFile}:"
        cat "$pixelsPlotFile"
    fi

    pushd "$d"

    if [ "$recentGnuplot" == "1" ]
    then
        echo "Running gnuplot..."
        gnuplot "$(basename $plotFile)"
        gnuplot "$(basename $pixelsPlotFile)"
    fi

    if [ -x "$(which julia)" ]
    then
        echo "Generating a histogram of relative errors..."
        "${DOUBLE_LENS_HIST}" "$(basename $outFile)" 
    fi

    popd
    
    echo "Removing files..."
    rm -vf "${files[@]}"
    if [ $clean -eq 1 ]
    then
        rm -vf "${inWorkFile}"
        rm -vf "${numRaysFile}"
    fi
done 
