#!/usr/bin/env gnuplot

reset

cut = 1
drawEps = 1

#nolap = system("which nolap.plt")
call "nolap.plt" "$0"

#lap = system("which lap.plt")
call "lap.plt" "$0"

reset
