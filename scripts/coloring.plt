#!/usr/bin/env gnuplot

if (!exists("cut")) {
    cut = 1
    s = sprintf("cut = %g", cut)
    print s
}

set palette defined ( \
    -cut 0 0 1, \
    -0.1*cut 0.25 .25 1, \
    -.01*cut .5 .5 1, \
    -.001*cut .75 .75 1, \
    0 "white", \
    0.001*cut 1 0.89 .75, \
    0.01*cut 1 0.77 .5, \
    0.1*cut 1 0.66 .25, \
    cut 1 0.55 0  \
)

unset colorbox
unset xtics
unset ytics
unset cbtics

