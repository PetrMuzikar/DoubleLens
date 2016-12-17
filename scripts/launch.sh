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
    utf-old|utf-cluster)
        SELECT="$1"
        kMax=46
        shift
        ;;
    cronus)
        SELECT="$1"
        kMax=20
        shift
        ;;
    *)
        SELECT="generic"
        kMax=4
esac

echo "$@"

for d in "$@"
do
    echo "Starting computations: ${d}."
    "${DOUBLE_LENS_SETUP}" "$SELECT" "$d" && "$DOUBLE_LENS_SUBMIT" "$SELECT" "$d" && echo "Computing..."
done
