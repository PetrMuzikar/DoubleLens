#!/usr/bin/env gnuplot

if (!exists("cut")) {
    cut = 0.1
    s = sprintf("cut = %g", cut)
    print s
}

mini = 0.0001*cut

set cbrange [-6:6]
set palette maxcolors 12
set palette defined (-6 "yellow", -5 "yellow", -5 "blue", -1 "white", 1 "white", 5 "red", 5 "green", 6 "green")
set colorbox

set cbtics ( \
    sprintf("< %g", -cut) -6, \
    sprintf("%g", -cut) -5, \
    sprintf("%g", -0.1*cut) -4, \
    sprintf("%g", -0.01*cut) -3, \
    sprintf("%g", -0.001*cut) -2, \
    sprintf("%g", -0.0001*cut) -1, \
    "0" 0, \
    sprintf("%g", 0.0001*cut) 1, \
    sprintf("%g", 0.001*cut) 2, \
    sprintf("%g", 0.01*cut) 3, \
    sprintf("%g", 0.1*cut) 4, \
    sprintf("%g", cut) 5, \
    sprintf("> %g", cut) 6 \
)

set colorbox
