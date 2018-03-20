#!/usr/bin/env gnuplot

if (!exists("cut")) {
    cut = 0.1
    s = sprintf("cut = %g", cut)
    print s
}

mini = 0.001*cut

set cbrange [-5:5]
set palette maxcolors 10
set palette defined (-5 "yellow", -4 "yellow", -4 "blue", -1 "white", 1 "white", 4 "red", 4 "green", 5 "green")
set colorbox

set cbtics ( \
    sprintf("< %g", -cut) -5, \
    sprintf("%g", -cut) -4, \
    sprintf("%g", -0.1*cut) -3, \
    sprintf("%g", -0.01*cut) -2, \
    sprintf("%g", -0.001*cut) -1, \
    "0" 0, \
    sprintf("%g", 0.001*cut) 1, \
    sprintf("%g", 0.01*cut) 2, \
    sprintf("%g", 0.1*cut) 3, \
    sprintf("%g", cut) 4, \
    sprintf("> %g", cut) 5 \
)

set colorbox
