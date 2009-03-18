#!/bin/bash
# new: use full path and libpath from invoking shell JA
# $1: GO4SYS:ROOTSYS (required by some root defaults!)
# $2: PATH
# $3: LD_LIBRARY_PATH
# $4: User top working directory
# $5: User analysis executable (relative to $3)
# $6: User analysis client name
# $7: hostname of GUI
# $8: connector port number
# $9: Analysis host name
IFSOLD=$IFS
IFS=':'
NOF=1
for xxx in $1
do
if [ $NOF == 1 ]
then GO4SYS=$xxx
fi
if [ $NOF == 2 ]
then ROOTSYS=$xxx
fi
NOF=$((NOF+1))
done
IFS=$IFSOLD
PATH=$2
LD_LIBRARY_PATH=$3
USERDIR=$4
export USERDIR
export LD_LIBRARY_PATH
export PATH
export ROOTSYS
export DISPLAY
export GO4SYS
echo --------- Starting Analysis as Server -----------
echo Name: $6
echo Host: $9
echo Dir:  $4
echo Exe:  $5
echo -------------------------------------------------
cd $USERDIR
. ./AnalysisStart.sh "$5 -server" $6 $7 $8 $8 $8
#sleep 10000
