#!/bin/sh
# new: use full path and libpath from invoking shell JA
# $1: GO4SYS
# $2  ROOTSYS
# $3: PATH
# $4: LD_LIBRARY_PATH
# $5: User top working directory
# $6: User analysis executable (relative to $3)
# $7: User analysis client name
# $8: hostname of GUI
# $9: connector port number
# $10: Analysis host name
GO4SYS=$1
ROOTSYS=$2
PATH=$3
LD_LIBRARY_PATH=$4
USERDIR=$5
export USERDIR
export LD_LIBRARY_PATH
export PATH
export ROOTSYS
#export DISPLAY
export GO4SYS
echo --------- Starting Analysis as Server -----------
echo Name: $7
echo Host: ${10}
echo Dir:  $5
echo Exe:  $6
echo -------------------------------------------------
cd $USERDIR
. ./AnalysisStart.sh "$6 -server" $7 $8 $9 $9 $9
#sleep 10000
