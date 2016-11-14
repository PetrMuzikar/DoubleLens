#!/usr/bin/env gnuplot

reset
f="../nothing.dat"

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

if (cut == 1) {
    set palette defined (-1 0 0 1, -.1 0.25 .25 1, -.01 .5 .5 1, \
    -.001 .75 .75 1, 0 "white", 0 "white", 0.001 1 0.89 .75, \
    0.01 1 0.77 .5, 0.1 1 0.66 .25, 1 1 0.55 0)
} 
if (cut == 0.1) {
    set palette defined (-0.1 0 0 1, -.01 0.25 .25 1, -.001 .5 .5 1, \
    -.0001 .75 .75 1, 0 "white", 0 "white", 0.0001 1 0.89 .75, \
    0.001 1 0.77 .5, 0.01 1 0.66 .25, 0.1 1 0.55 0)
}
if (cut == 0.01) {
    set palette defined (-0.01 0 0 1, -.001 0.25 .25 1, -.0001 .5 .5 1, \
    -.00001 .75 .75 1, 0 "white", 0 "white", 0.00001 1 0.89 .75, \
    0.0001 1 0.77 .5, 0.001 1 0.66 .25, 0.01 1 0.55 0)
}

unset colorbox
unset xtics
unset ytics
#splot (sin(x/y))**5
splot input using ($$3-cm):4:11 notitle with pm3d

if (cut == 1) {
    set cbtics ("-1" -1, "-0.1" -.8, "-0.01" -.6, "-0.001" -.4, "" -.2, \
        "0" 0, "" .2, "0.001" .4, "0.01" .6, "0.1" .8, "1" 1)
    set palette defined (-1 0 0 1, -.8 0.25 .25 1, -.6 .5 .5 1, \
        -.4 .75 .75 1, -.2 "white", 0.2 "white", .4 1 0.89 .75, \
        .6 1 0.77 .5, .8 1 0.66 .25, 1 1 0.55 0)
} 
if (cut == 0.1) {
    set cbtics ("-0.1" -0.1, "-0.01" -.08, "-0.001" -.06, "-0.0001" -.04, \
        "" -.02, "0" 0, "" .02, "0.0001" .04, "0.001" .06, "0.01" .08, \
        "0.1" 0.1)
    set palette defined (-0.1 0 0 1, -.08 0.25 .25 1, -.06 .5 .5 1, \
        -.04 .75 .75 1, -.02 "white", 0.02 "white", .04 1 0.89 .75, \
        .06 1 0.77 .5, .08 1 0.66 .25, 0.1 1 0.55 0)
}
if (cut == 0.01) {
    set cbtics ("-0.01" -0.01, "-0.001" -.008, "-0.0001" -.006, \
        "-1e-5" -.004, "" -.002, "0" 0, "" .002, "1e-5" .004, \
        "0.0001" .006, "0.001" .008, "0.01" 0.01)
    set palette defined (-0.01 0 0 1, -.008 0.25 .25 1, -.006 .5 .5 1, \
        -.004 .75 .75 1, -.002 "white", 0.002 "white", .004 1 0.89 .75, \
        .006 1 0.77 .5, .008 1 0.66 .25, 0.01 1 0.55 0)
}

set colorbox
#set xtics xtStep
#set ytics ytStep
set xtics
set ytics
set xlabel "{/Symbol b}_x"
set ylabel "{/Symbol b}_y"
set title "Relative difference between magnifications after the Laplace-correction for " . addtit
splot f using 1:2:($$3-2) notitle

unset multiplot
unset out

# default terminal defined in the file .gnuplot
@DEFAULT
