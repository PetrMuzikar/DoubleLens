#!/bin/bash

for d in "$@"
do
    echo "Starting computations: ${d}."
    ./setup.sh "$d" && ./submit.sh "$d"
done
