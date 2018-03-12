#!/usr/bin/env gnuplot

reset

print "Processing " . ARG0 . "..."

call "nolap.plt" "@ARG1"

call "lap.plt" "@ARG1"

#reset
