#!/usr/bin/env gnuplot

reset

cut = 1
drawEps = 1

nolap = system("which nolap.plt")
call nolap "$0"

lap = system("which lap.plt")
call lap "$0"

reset
