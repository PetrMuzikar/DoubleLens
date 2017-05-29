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
outname3 = basename . "-rel2.eps"
dataname = basename . ".dat"
diffConfFile = basename . "-conf.plt"

addtit = sprintf("m_1 = %g, d = %s, {/Symbol b} = %s", m1, d, beta)

@EPS

set view map
set pm3d at t corners2color c1
unset surface
set size ratio -1
set title ""
set lmargin 0.1
set bmargin 0.1
set size 1.2, 1.2

absPlot = 1
relPlot = 0
rel2Plot = 0
if (diffConf == 1) {
    load diffConfFile
}

set out outname
set title "Absolute difference between magnifications for " . addtit
load "diverging_map_gnuplot.pal"

splot input using 3:4:(column(11)-column(6)) notitle with pm3d

unset out

absPlot = 0
relPlot = 1
rel2Plot = 0
if (diffConf == 1) {
    load diffConfFile
}

set out outname2
set title ""

set multiplot

set xrange [xmin:xmax]
set yrange [ymin:ymax]
set cbrange [-cut:cut]

load "coloring.plt"

splot input using 3:4:((column(11)-column(6))/column(6)) notitle with pm3d

zmin = GPVAL_Z_MIN
zmax = GPVAL_Z_MAX
set zrange [zmin:zmax]
load "colorbar.plt"

set xlabel "{/Symbol b}_x"
set ylabel "{/Symbol b}_y"
set title "Relative difference between magnifications for " . addtit

#replot
splot -10 notitle

unset multiplot
unset out

unset zrange
unset colorbox
unset cbtics
absPlot = 0
relPlot = 0
rel2Plot = 1
if (diffConf == 1) {
    load diffConfFile
}
load "col3.plt"

set out outname3

splot input using 3:4:((column(11)-column(6))/column(6)) notitle with pm3d

unset out

# default terminal defined in the file .gnuplot
@DEFAULT

