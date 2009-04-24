#!/bin/bash
# new: use full path and libpath from invoking shell JA
# $1: GO45SYS
# $2  ROOTSYS (required by some root defaults!)
# $3: PATH
# $4: LD_LIBRARY_PATH
# $5: User top working directory
# $6: User analysis executable (relative to $3)
# $7: User analysis client name
# $8: hostname of GUI
# $9: connector port number
# $10: analysis host
GO4SYS=$1
ROOTSYS=$2
PATH=$3
LD_LIBRARY_PATH=$4
USERDIR=$5
export USERDIR
export LD_LIBRARY_PATH
export PATH
export ROOTSYS
export DISPLAY
export GO4SYS
echo --------- Starting Analysis as Client -----------
echo Name: $7
echo Host: ${10}
echo Dir:  $5
echo Exe:  $6
echo -------------------------------------------------
cd $USERDIR
. ./AnalysisStart.sh "$6 -gui" $7 $8 $9 2>&1
#sleep 10000
