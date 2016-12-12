#!/bin/bash

shopt -s nullglob
shopt -s extglob

# set no limits for STACK size
ulimit -s unlimited

case "$SELECT" in
    utf)
        # submit this script with 
        # ~$ qsub -cwd -l h_vmem=XXg,h_fsize=XXg job.sh
        #
        # h_vmem ... required memory
        # h_fsize .... required scratch space

        echo This calculation was done on `hostname`

        #Remember the submit directory for later
        SbmtDir=$PWD

        # Check where we are - if in $HOME, qsub called without -cwd
        if [ $PWD == $HOME ];
        then
            echo "Call qsub with -cwd"
            exit
        fi

        #THIS SCRIPT IS BEING EXECUTED ON THE NODE, ALL THE I/O SHOULD USE LOCAL DISC
        #DRIVE /SCRATCH
        # Here, we create the scratch directory for the job, copy all the data from
        # submit directory there, and move there to execute the calculation
        export SCRATCH=/scratch/$USER/$JOB_ID
        mkdir -p $SCRATCH
        cp -r * $SCRATCH/.
        cd $SCRATCH
        CurrDir=$PWD

        DOUBLE_LENS_JOB="$(which ${DOUBLE_LENS_JOB:-job.sh})"
        echo "DOUBLE_LENS_JOB=${DOUBLE_LENS_JOB}"
        echo "PATH=$PATH"
        echo "$(env)"

        if [ ! -x "$DOUBLE_LENS_JOB" ]
        then
            echo "$DOUBLE_LENS_JOB is not executable!"
            exit 1 
        fi

        # HERE COMES THE ACTUAL CALCULATION - CALL OF THE MAIN BINARY ETC.:
        inFiles=(*-+([[:digit:]])\.dat)
        f=${inFiles[0]}
        "$DOUBLE_LENS_JOB" "$EXEC" "$f"
        ex="$?"

        # DON'T FORGET TO REMOVE AUX. FILES BEFORE COPYING DATA BACK TO $HOME 
        rm -f shooting.sh.*
        if [ "$ex" -eq 0 ]
        then
            rm -f err.dat
        else
            echo "Error while running $DOUBLE_LENS_JOB!"
            exit "$ex"
        fi

        # COPY THE RESULTS BACK TO THE SUBMIT DIR IN $HOME
        mkdir -p $SbmtDir/$JOB_ID
        cd $CurrDir
        cp -r * $SbmtDir/$JOB_ID/. && rm -rf *
        #cd $SbmtDir
        #rm -f shooting.sh.*

        # CLEAN THE SCRATCH
        rmdir $SCRATCH && echo "scratch cleaned"
        ;;

    cronus)
        echo This calculation was done on `hostname`

        PATH="$PBS_O_PATH"

        cd "$PBS_O_WORKDIR"

        DOUBLE_LENS_JOB="$(which ${DOUBLE_LENS_JOB:-job.sh})"
        echo "DOUBLE_LENS_JOB=${DOUBLE_LENS_JOB}"
        echo "PATH=$PATH"
        echo "$(env)"

        if [ ! -x "$DOUBLE_LENS_JOB" ]
        then
            echo "$DOUBLE_LENS_JOB is not executable!"
            exit 1 
        fi

        # HERE COMES THE ACTUAL CALCULATION - CALL OF THE MAIN BINARY ETC.:
        inFiles=(*-+([[:digit:]])\.dat)
        f=${inFiles[0]}
        "$DOUBLE_LENS_JOB" "$EXEC" "$f"
        ex="$?"

        if [ "$ex" -eq 0 ]
        then
            rm -f err.dat
        else
            echo "Error while running $DOUBLE_LENS_JOB!"
            exit "$ex"
        fi
        ;;
    *)
        echo "Please specify whether we are computing on utf cluster or on cronus."
        exit 1
esac
