#!/usr/bin/env gnuplot

reset

if (drawEps) {
    @EPS
    set out outname2
}
if (!drawEps) {
    unset out
}

set view map
set pm3d at t corners2color c1
unset surface
set size ratio -1
set title ""
set lmargin 0.1
set bmargin 0.1
set size 1.2, 1.2

if (plotConf == 1) {
    load plotConfFile
}

set multiplot
set xrange [xmin:xmax]
set yrange [ymin:ymax]
set zrange [-cut:cut]
set cbrange [-cut:cut]

load "coloring.plt"

splot input using ($$3-cm):4:11 notitle with pm3d

load "colorbar.plt"

set xlabel "{/Symbol b}_x"
set ylabel "{/Symbol b}_y"
set title "Relative difference between magnifications after the Laplace-correction for " . addtit
splot -10 notitle

unset multiplot
unset out

# default terminal defined in the file .gnuplot
@DEFAULT
