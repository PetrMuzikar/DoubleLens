#/bin/bash

shopt -s nullglob
shopt -s extglob

drawDom="drawDomains.awk"
inp="input.awk"

for d in $@
do
    outFileBaseName="${d/%-out/}"
    
    inPrefix="${HOME}/Submit/inFiles/${outFileBaseName}-in"
    
    inFile="${inPrefix}/${outFileBaseName}-in.dat"
    confFile="${inPrefix}/${outFileBaseName}-conf.sh"
    domainsFile="${inPrefix}/${outFileBaseName}-domains.dat"
    testImagesFile="${inPrefix}/${outFileBaseName}-images.dat"
    plotConfFile="${inPrefix}/${outFileBaseName}-conf.plt"
    
    outFile="${d}/${outFileBaseName}-out.dat"
    plotFile="${d}/${outFileBaseName}-plot.plt"
    pixelsFile="${d}/${outFileBaseName}-pixels.dat"
    pixelsPlotFile="${d}/${outFileBaseName}-pixels.plt"
    inWorkFile="${d}/inWork.dat"
    #testFileBaseName="test"
    #testFile="${testFileBaseName}.dat"
    #dataFile="data.dat"
    
    plotMaps="../maps.plt"
    
    DoubleLens="${HOME}/Lensing/DoubleLens/bin/Release/DoubleLens"
    
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
    
    prefix="${HOME}/Lensing/DoubleLens"
    choice=$(( random + 2 * integ ))
    case $choice in
        0) executable="${prefix}/bin/Release/DoubleLensSobolSimple" ;;
        1) executable="${prefix}/bin/Release/DoubleLensRandomSimple" ;;
        2) executable="${prefix}/bin/Release/DoubleLensSobolInteg" ;;
        3) executable="${prefix}/bin/Release/DoubleLensRandomInteg" ;;
        *) executable="none" ;;
    esac
    
    if [ -x $executable ]
    then
        echo "Using $executable"
    else
        echo "$executable is not executable!"
        exit 2
    fi
    
    echo "Combining results..."
    files=("${d}/${outFileBaseName}"-+([[:digit:]])-out.dat)
    for f in ${files[@]}
    do
        echo $f
    done
    tail -n 3 "$testImagesFile" | awk 'BEGIN{r=0;s=0} /^# rays=/{r = $3} /^# Ssum=/{s = $3} END{printf("r %.12e 0 1 0 %.12e %d ", s, s, r)}' > $inWorkFile 
    if [ $random -eq 0 ]
    then
        echo "0" >> "$inWorkFile"
    fi
    cat "$inFile" >> "$inWorkFile"
    if [ "$pixels" != "" ]
    then
        echo "$pixels" >> "$inWorkFile"
    fi
    "$executable" c "$outFile" "$inWorkFile" "${files[@]}"
    for f in ${files[@]}
    do
        cat "$f" | awk '/^# Time:/{ printf("# computation time: %e s = %.2f h\n", $3, $3 / 3600)} {next;}' >> "$outFile"
    done
    cat "$outFile" | awk 'BEGIN{t=0} /^# computation/{ t += $4 } {next} END{ printf("# total time: %e s = %.2f h\n", t, t / 3600) }' >> "$outFile"
    
    if [ "${pixels}" != "" ]
    then
        echo "Collecting informations about rays recorded in selected pixels..."
        cat "${files[0]}" | grep "^#p" | sed 's/^#p//g' | tee "$pixelsFile"
        grep -h "^#rtp" "${files[@]}" | sed 's/^#rtp//g' | sort -k1n,2 | awk 'BEGIN{n=0;} /^$|^#/{next} {n++; if (n == 1) {i = $1; j = $2; print; next;} if ((i != $1) || (j != $2)) { i=$1; j=$2; print "\n"; print $0; } else {print $0} }' >> "$pixelsFile"
    fi
    
    echo "Creating gnuplot file(s)..."
    echo "plotConf = ${plotConf}" > "$plotFile"
    echo "plotConfFile = \"$(basename ${plotConfFile})\"" >> "$plotFile"
    #if [ "$plotConf" == "1" ]
    #then
    #    cat "$plotConfFile" > "$plotFile"
    #fi
    echo -n "call \"${plotMaps}\" \"$(basename ${d}/${outFileBaseName}-out)\" " >> $plotFile
    head "${outFile}" | grep "##" | "${inp}" -v "todo=0" -v "random=${random}" >> $plotFile
    echo "File ${plotFile}:"
    cat $plotFile
    #gnuplot $plotFile
    
    if [ "${pixels}" != "" ]
    then
        echo "unset key" > "$pixelsPlotFile"
        "${drawDom}" "$domainsFile" >> "$pixelsPlotFile"
        echo "p \"$(basename ${pixelsFile})\" u 5:6:(column(-2)+1) lc variable" >> "$pixelsFile" >> "$pixelsPlotFile"
        echo "File ${pixelsPlotFile}:"
        cat "$pixelsPlotFile"
    fi
    
    echo "Removing files..."
    rm -vf "${inWorkFile}"
done 
