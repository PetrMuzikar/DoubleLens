#!/usr/bin/env gnuplot

mini = @ARG1
maxi = @ARG2
draw = ARG3
tit = ARG4

set multiplot

set pm3d map
set pm3d corners2color mean
set size ratio -1
#set size 1.2, 1.2
set palette defined (0 "blue", 0.5 "gray", 1 "red")
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
set autoscale cb

set pm3d corners2color max
set palette defined (0 "green", 1 "green")
set autoscale z
set zrange [maxi:]
@draw

set pm3d corners2color min
set palette defined (0 "yellow", 1 "yellow")
set autoscale z
set zrange [:mini]
@draw

unset multiplot

