#!/usr/bin/env gnuplot

reset

set macro

zmax = 0
logz = 1
cont = 0
zmin = 1
cbmin = zmin
absPlot = 0
relPlot = 0
cut = 1

f(x,y) = x / (x + (1 - y) * (1 - x))

mu1 = "0.5"
m1 = "0.5"
d = "1.9"
beta = "0.01"
#f="nothing.dat"

argc = "$#"
if (argc == 0) { 
    basename = "out"
}

if (argc >= 1) { 
    input = "$0.dat"
    basename = "$0"
}

if (argc >= 2) {
    mu1 = "$1"
}

if (argc >= 3) {
    d = "$2"
}

if (argc >= 4) {
    beta = "$3"
}

if (argc >= 5) { 
    xmin = "$4"
}

if (argc >= 6) { 
    xmax = "$5"
}

if (argc >= 7) { 
    ymin = "$6"
}

if (argc >= 8) { 
    ymax = "$7"
}

m1 = f(mu1, beta)

print "mu1 = " . mu1
print "m1 = " . sprintf("%f",m1)
print "d = " . d
print "beta = " . beta

outname = basename . "-abs.eps"
outname2 = basename . "-rel.eps"
dataname = basename . ".dat"
diffConfFile = basename . "-conf.plt"

addtit = sprintf("m_1 = %g, d = %s, {/Symbol b} = %s", m1, d, beta)

# Reading variables
if (diffConf == 1) {
    load diffConfFile
}

set view map
set pm3d at t corners2color c1
unset surface
set size ratio -1
set title ""
set lmargin 0.1
set bmargin 0.1
set size 1.2, 1.2

if (diffConf == 1) {
    load diffConfFile
}

set xlabel "{/Symbol b}_x"
set ylabel "{/Symbol b}_y"

set term postscript eps color solid enhanced

set out outname
set title "Absolute difference between magnifications for " . addtit
load "diverging_map_gnuplot.pal"
absPlot = 1
relPlot = 0
if (diffConf == 1) {
    load diffConfFile
}
splot input using 3:4:($$6-$$11) notitle with pm3d

set out outname2
set title "Relative difference between magnifications for " . addtit
absPlot = 0
relPlot = 1
set zrange [-cut:cut]
set cbrange [-cut:cut]

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

if (diffConf == 1) {
    load diffConfFile
}

splot input using 3:4:(($$6-$$11)/$$11) notitle with pm3d

unset out
set term wxt

