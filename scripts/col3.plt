#!/usr/bin/env gnuplot

#reset

if (!exists("maxi")) {
    maxi = 1
}
mini = -maxi

if (!exists("ma")) {
    ma = 0.5
}
mi = -ma

#set zrange [mini:maxi]
set cbrange [mini:maxi]
set palette defined (mini "blue", mi "blue", mi "gray", ma "gray", ma "yellow", maxi "yellow")

#set isosamples 100,100
#set pm3d map
#sp 2*sin(x)
