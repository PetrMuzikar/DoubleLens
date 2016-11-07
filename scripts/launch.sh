#!/bin/bash

echo "DOUBLE_LENS_SCRIPTS=$DOUBLE_LENS_SCRIPTS"

DOUBLE_LENS_SETUP="$DOUBLE_LENS_SCRIPTS/setup.sh"
echo "DOUBLE_LENS_SETUP=$DOUBLE_LENS_SETUP"
if [ ! -x "$DOUBLE_LENS_SETUP" ]
then
    echo "DOUBLE_LENS_SETUP=${DOUBLE_LENS_SETUP}\nPlease set the correct path to the script shooting.sh."
fi

DOUBLE_LENS_SUBMIT="$DOUBLE_LENS_SCRIPTS/submit.sh"
echo "DOUBLE_LENS_SUBMIT=$DOUBLE_LENS_SUBMIT"
if [ ! -x "$DOUBLE_LENS_SUBMIT" ]
then
    echo "DOUBLE_LENS_SUBMIT=${DOUBLE_LENS_SUBMIT}\nPlease set the correct path to the script shooting.sh."
fi

case "$1" in
    -l|--local|-c|--cluster)
    c="$1"
    shift
    ;;
    *)
    c=""
    ;;
esac

echo "$@"

for d in "$@"
do
    echo "Starting computations: ${d}."
    if [ "$c" != "" ]
    then
        "${DOUBLE_LENS_SETUP}" "$c" "$d" && "$DOUBLE_LENS_SUBMIT" "$c" "$d" && echo "Computing..."
    else
        "${DOUBLE_LENS_SETUP}" "$d" && "$DOUBLE_LENS_SUBMIT" "$d" && echo "Computing..."
    fi
done
