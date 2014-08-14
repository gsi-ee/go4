#!/bin/bash

# Script to create a ROOT dictionary
# Called by Makefile.

GO4_OS=${1}
DLLSUF=${2}
ROOTEXEPATH=${3}
ROOTINTERPRETER=${4}
DICTIONARY=${5}
CXXOPTIONS=${6}

HEADERS=""
LIBNAME=""

while [[ "x$7" != "x" ]]; do
   fileext="${7##*.}"
   if [[ "$fileext" == "h" ]]; then 
      HEADERS+=" $7"
   elif [[ "$fileext" == "$DLLSUF" && "x$LIBNAME" == "x" ]]; then
      LIBNAME=$7
#      echo "Found library name $LIBNAME"
   else
      echo "Unknown file name $7"
   fi
   shift
done

if [[ "$GO4_OS" == "Win32" ]]; then
  rm -f *._rootcint *_cint 
  CINT=$ROOTINTERPRETER
  CINTOPT="-c -p"
else
  CINT=$ROOTEXEPATH/bin/$ROOTINTERPRETER
  CINTOPT="-c -p"
fi  

if [[ "$ROOTINTERPRETER" == "rootcint" ]]; then

   echo "Generating root5 dictionary $DICTIONARY ..." 

   $CINT -f $DICTIONARY $CINTOPT $CXXOPTIONS $HEADERS
   
   exit 0
fi

echo "Generating root6 dictionary $DICTIONARY ..." 

if [[ "x$LIBNAME" == "x" ]]; then 
  LIBNAME="libGo4UserAnalysis.$DLLSUF" 
fi   

#$CINT -f $DICTIONARY -s $LIBNAME -rml $(basename "$LIBNAME") -c $CXXOPTIONS $HEADERS

$CINT -f $DICTIONARY -s $LIBNAME -c $CXXOPTIONS $HEADERS

   
exit 0
