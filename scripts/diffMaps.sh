#!/bin/bash

shopt -s extglob

fCompare1="$1"
fCompare2="$2"
#fOutBaseName="$3"
diffConf="0"
random="0"

n1="${#fCompare1}"
n2="${#fCompare2}"
n="$n1"
if [ "$n2" -lt "$n" ]
then
    n="$n2"
fi
j=0
while [ "$j" -lt "$n" ] && [ "${fCompare1:$j:1}" == "${fCompare2:$j:1}" ]
do
    j=$(( j + 1 ))
done
common="${fCompare1:0:$j}"
fCompare1Suff="${fCompare1:$j}"
fCompare2Suff="${fCompare2:$j}"
fOut="${common}${fCompare1Suff%-out.dat}-${fCompare2Suff%-out.dat}-diff.dat"
fOutBaseName="${fOut%.dat}"
fPlot="${fOut/%.dat/-plot.plt}"
#fConf="${fOut/%.dat/-conf.sh}"
fCompare1Conf="${fCompare1/%out.dat/conf.sh}"
fPlotConf="${fOut/%.dat/-conf.plt}"
diffMaps="diffMaps.plt"

if [ -r "$fPlotConf" ]
then
    diffConf="1"
    cat "$fPlotConf"
fi

if [ -r "$fCompare1Conf" ]
then
    source "$fCompare1Conf"
fi

#if [ -r "$fConf" ]
#then
#    source "$fConf"
#fi

h="40"
fTemp1="t1.dat"
fTemp2="t2.dat"

m1=$(head -n "$h" "$fCompare1" | awk '/^#[[:blank:]]+m1 = /{print $4; next;}')
m1Ref=$(head -n "$h" "$fCompare2" | awk '/^#[[:blank:]]+m1 = /{print $4; next;}')
if [ "$m1" != "$m1Ref" ]
then
    echo "m1 = ${m1} != ${m1Ref}"
    exit 1
fi

d=$(head -n "$h" "$fCompare1" | awk '/^#[[:blank:]]+d =/{print $4}')
dRef=$(head -n "$h" "$fCompare2" | awk '/^#[[:blank:]]+d =/{print $4}')
if [ "$d" != "$dRef" ]
then
    echo "d = ${d} != ${dRef}"
    exit 1
fi

beta=$(head -n "$h" "$fCompare1" | awk '/^#[[:blank:]]+beta/{print $4}')
betaRef=$(head -n "$h" "$fCompare1" | awk '/^#[[:blank:]]+beta/{print $4}')
if [ "$beta" != "$betaRef" ]
then
    echo "beta = ${beta} != ${betaRef}"
    exit 1
fi

echo "$m1 $d $beta"

awk '/^#/{next;} /^[[:blank:]]+$/{print; next;} { for (i = 1; i <= 9; i++) {printf("%s ", $i)} printf("\n")}' "$fCompare1" > "$fTemp1"
awk '/^#/{next;} /^[[:blank:]]+$/{print; next;} { for (i = 5; i <= 9; i++) {printf("%s ", $i)} printf("\n")}' "$fCompare2" > "$fTemp2"

echo "# Files: $fCompare1 $fCompare2" > "$fOut"
paste "$fTemp1" "$fTemp2" | awk '/^[\s\t]*$/{printf("\n"); next;} {print}' >> "$fOut"

echo "Creating gnuplot file and executing it..."
echo "diffConf = ${diffConf}" > "$fPlot"
echo "diffConfFile = \"${diffConfFile}\"" >> "$fPlot"
echo -n "call \"./${diffMaps}\" \"${fOutBaseName}\" " >> "$fPlot"
head "${fCompare1}" | grep "##" | ./input.awk -v "todo=0" -v "random=${random}" >> "$fPlot"
cat "$fPlot"
gnuplot "$fPlot"

rm -vf "$fTemp1" "$fTemp2"
