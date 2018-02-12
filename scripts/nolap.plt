#!/usr/bin/env gnuplot

reset

if (drawEps) {
    @EPS
    set out outname
}
if (!drawEps) {
    unset out
}

cut = 0.1

set pm3d map
set pm3d corners2color c1
set size ratio -1
set title ""
set lmargin 0.1
set bmargin 0.1
set size 1.2, 1.2
set size ratio -1
set log zcb
set xrange [xmin:xmax]
set yrange [ymin:ymax]
unset colorbox
unset cbtics

if (plotConf == 1) {
    load plotConfFile
}

mini = 0.0001*cut
maxi = cut
set cbrange [mini:maxi]

set multiplot

unset xtics
unset ytics
set palette defined (0 "white", 1 "red")
set palette maxcolors 4
set zrange [mini:maxi]
sp input u 3:4:10 notitle

set palette defined (0 "white", 1 "blue")
set palette maxcolors 4
set zrange [mini:maxi]
sp input u 3:4:($$10 < 0 ? abs($$10) : 0) notitle

set palette defined (0 "green", 1 "green")
set palette maxcolors 2
set zrange [mini:maxi]
sp input u 3:4:(abs($$10) > cut ? maxi : maxi+1) notitle

unset log cb
set cbrange [-maxi:maxi]
load "colorbar.plt"

set xtics
set ytics
set xlabel "{/Symbol b}_x"
set ylabel "{/Symbol b}_y"
set title "Relative difference between magnifications for " . addtit
sp (maxi+1) notitle

unset multiplot

unset out

# default terminal defined in the file .gnuplot
@DEFAULT
