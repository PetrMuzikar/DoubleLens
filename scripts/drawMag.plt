#!/usr/bin/env gnuplot

mini = @ARG1
maxi = @ARG2
draw = ARG3
tit = ARG4

under = -8.9e307
upper = 8.9e307
if (logz == 1) {
    under = 1e-16
}

set multiplot

set pm3d map
set pm3d corners2color c1
set size ratio -1
#set palette defined (0 "blue", 0.5 "gray", 1 "red")
load "jet.pal"
set autoscale z
set cbrange [mini:maxi]
set colorbox
set xtics
set ytics
set xlabel "{/Symbol b}_x"
set ylabel "{/Symbol b}_y"
unset key
set title tit
@draw

unset colorbox
unset xtics
unset ytics
unset xlabel
unset ylabel
set title ""

#set pm3d corners2color max
set palette defined (0 "pink", 1 "pink")
set zrange [maxi:upper]
set cbrange [maxi:upper]
@draw

#set pm3d corners2color min
set palette defined (0 "purple", 1 "purple")
set zrange [under:mini]
set cbrange [under:mini]
@draw

unset multiplot

