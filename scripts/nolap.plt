#!/usr/bin/env gnuplot

reset

script = nolap

print "Processing " . ARG0 . "..."

if (ft eq "EPS") {
    @EPS
}
if (ft eq "PDF") {
    @PDF
}
set out imgrelname

cut = 0.1

c(x, y) = abs(x) >= y ? sgn(x)*(log10(abs(x)/y)+1) : 0

set pm3d map
set pm3d corners2color c1
set size ratio -1
set title ""
set lmargin 0.1
set bmargin 0.1
#set size 1.2, 1.2
set size ratio -1
#set log zcb
set xrange [xmin:xmax]
set yrange [ymin:ymax]
#set cbrange [-6:6]
#set palette maxcolors 12
#set palette model RGB
#set palette defined (\
#    -6 "yellow", \
#    -5 "yellow", \
#    -5 0.0 0.0 1.0, \
#    -4 0.25 0.25 1.0, \
#    -3 0.5 0.5 1.0, \
#    -2 0.75 0.75 1.0, \
#    -1 "white", \
#    0 "white", \
#    1 "white", \
#    2 1.0 0.75 0.75, \
#    3 1.0 0.5 0.5, \
#    4 1.0 0.25 0.25, \
#    5 1.0 0.0 0.0, \
#    5 "green", \
#    6 "green" \
#)
#set colorbox
set xtics
set ytics
set xlabel "{/Symbol b}_x"
set ylabel "{/Symbol b}_y"
set title "Relative difference between magnifications for " . addtit

if (plotConf == 1) {
    load plotConfFile
}

load "colorbar.plt"

sp input u 3:4:(c($10,mini)) notitle

unset out

@DEFAULT
