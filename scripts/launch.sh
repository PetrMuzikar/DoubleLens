#!/bin/bash

DOUBLE_LENS_SETUP="$(which ${DOUBLE_LENS_SETUP:-setup.sh})"
if [ ! -x "$DOUBLE_LENS_SETUP" ]
then
    "$DOUBLE_LENS_SETUP is not executable!"
    exit 1
else
    echo "DOUBLE_LENS_SETUP=$DOUBLE_LENS_SETUP"
fi

DOUBLE_LENS_SUBMIT="$(which ${DOUBLE_LENS_SUBMIT:-submit.sh})"
if [ ! -x "$DOUBLE_LENS_SUBMIT" ]
then
    "$DOUBLE_LENS_SUBMIT is not executable!"
    exit 1
else
    echo "DOUBLE_LENS_SUBMIT=$DOUBLE_LENS_SUBMIT"
fi

case "$1" in
    -l|--local|-c|--cluster)
        c="$1"
        shift
        ;;
esac

echo "$@"

for d in "$@"
do
    echo "Starting computations: ${d}."
    "${DOUBLE_LENS_SETUP}" "$c" "$d" && "$DOUBLE_LENS_SUBMIT" "$c" "$d" && echo "Computing..."
done
