#!/bin/bash

contourFile=$1

cat "${contourFile}" | awk 'BEGIN{c=0} /^$|^#/{next} {if (c!=$3) {c=$3; printf("set cbtics add (%.12e)\n", c)}}'

