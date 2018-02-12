#!/usr/bin/env gnuplot

if (!exists("cut")) {
    cut = 1
    s = sprintf("cut = %g", cut)
    print s
}

#set cbtics ( \
#    sprintf("%g", -cut) -cut, \
#    sprintf("%g", -0.1*cut) -0.75*cut, \
#    sprintf("%g", -0.01*cut) -0.5*cut, \
#    sprintf("%g", -0.001*cut) -0.25*cut, \
#    "0" 0, \
#    sprintf("%g", 0.001*cut) 0.25*cut, \
#    sprintf("%g", 0.01*cut) 0.5*cut, \
#    sprintf("%g", 0.1*cut) 0.75*cut, \
#    sprintf("%g", cut) cut \
#)

set palette maxcolors 10
set palette defined ( \
    -cut "blue", \
    -0.2*cut "white", \
    0.2*cut "white", \
    cut "red" \
)

#set palette defined ( \
#    -cut 0 0 1, \
#    -0.8*cut 0.25 .25 1, \
#    -0.6*cut .5 .5 1, \
#    -0.4*cut .75 .75 1, \
#    -0.2*cut "white", \
#    0.2*cut "white", \
#    0.4*cut 1 0.89 .75, \
#    0.6*cut 1 0.77 .5, \
#    0.8*cut 1 0.66 .25, \
#    cut 1 0.55 0 \
#)

#set palette defined ( \
#    -cut 0 0 1, \
#    -0.75*cut 0.25 .25 1, \
#    -0.5*cut .5 .5 1, \
#    -0.25*cut .75 .75 1, \
#    0 "white", \
#    0.25*cut 1 0.89 .75, \
#    0.5*cut 1 0.77 .5, \
#    0.75*cut 1 0.66 .25, \
#    cut 1 0.55 0 \
#)

set cbtics ( \
    sprintf("%g", -cut) -cut, \
    sprintf("%g", -0.1*cut) -.8*cut, \
    sprintf("%g", -0.01*cut) -.6*cut, \
    sprintf("%g", -0.001*cut) -.4*cut, \
    sprintf("%g", -0.0001*cut) -.2*cut, \
    "0" 0, \
    sprintf("%g", 0.0001*cut) .2*cut, \
    sprintf("%g", 0.001*cut) .4*cut, \
    sprintf("%g", 0.01*cut) .6*cut, \
    sprintf("%g", 0.1*cut) .8*cut, \
    sprintf("%g", cut) cut \
)

set colorbox
set xtics
set ytics
