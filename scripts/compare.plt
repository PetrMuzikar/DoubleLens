#!/usr/bin/env gnuplot

reset

cut = 1
drawEps = 1

call "../nolap.plt" "$0"

call "../lap.plt" "$0"

reset
