#!/bin/bash

EXEC="$1"
IN="$2"
OUT=${IN/%.dat/-out.dat}
#ERR=${IN/%.dat/-err.dat}
LOG=${IN/%.dat/-log.dat}

pwd > "$LOG"
ls -lsa >> "$LOG"
echo $EXEC >> "$LOG"
ldd $EXEC >> "$LOG"

if [ ! -x "$EXEC" ]; then
    echo "File '$EXEC' is not EXEC!" >> "$LOG"
    exit 1
fi

if [ ! -r "$IN" ]; then
    echo "Input file '$IN' is not readable!" >> "$LOG"
    exit 1
fi

$EXEC s "$OUT" "$IN" || echo "Error while running '$EXEC', exit status $?!" >> "$LOG"

echo "Job is finished." >> "$LOG"
