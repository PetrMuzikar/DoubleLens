#!/bin/bash

shopt -s nullglob
shopt -s extglob

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

# set no limits for STACK size
ulimit -s unlimited

# set the intel compiler and MKL environment variables if necessary
# source ~/intelvars.env

# set the number of threads available for multi-threaded MKL routines
# (number of cores actually used by the code, even though with the qsub
# command above SGE reserve 1 slot only. The kernel takes care of it, though...
# export MKL_NUM_THREADS=4

# set other environment variables - MOLCAS in this example
if [ -z "$LD_LIBRARY_PATH" ]; then
    export LD_LIBRARY_PATH=".:$HOME/lib"
else
    export LD_LIBRARY_PATH=".:$HOME/lib:$LD_LIBRARY_PATH"
fi

export LIBRARY_PATH=$LD_LIBRARY_PATH

DOUBLE_LENS_JOB="$(which ${DOUBLE_LENS_JOB:-job.sh)"

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
fi

# COPY THE RESULTS BACK TO THE SUBMIT DIR IN $HOME
mkdir -p $SbmtDir/$JOB_ID
cd $CurrDir
cp -r * $SbmtDir/$JOB_ID/. && rm -rf *
cd $SbmtDir
rm -f shooting.sh.*

# CLEAN THE SCRATCH
rmdir $SCRATCH && echo "scratch cleaned"

