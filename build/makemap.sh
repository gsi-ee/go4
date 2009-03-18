#! /bin/bash

# Script to add entries in map files of go4 classes
# Called by Makefile

GO4_OS=$1
RLIBMAP=$2
MAPNAME=$3
LIBNAME=$4
LINKDEFNAME=$5
DEPLIBS=$6

DUMMY1=$MAPNAME.dummy1
DUMMY2=$MAPNAME.dummy2

#  echo 'map name = ' $MAPNAME
#  echo 'lib name = ' $LIBNAME
#  echo 'linkdef name = ' $LINKDEFNAME

if [ "$GO4_OS" = "Win32" ]; then
  LIBNAME=${LIBNAME//.lib/.dll}
  DEPLIBS=${DEPLIBS//.lib/.dll}
fi

if [[ -a $LIBNAME ]]; then
  if [[ -a $LINKDEFNAME ]]; then
    $RLIBMAP -r $DUMMY1 -l $LIBNAME -d $DEPLIBS -c $LINKDEFNAME
    if [[ -a $MAPNAME ]]; then
      cat $MAPNAME $DUMMY1 > $DUMMY2
      mv -f $DUMMY2 $MAPNAME
      rm -f $DUMMY1
    else
      mv -f $DUMMY1 $MAPNAME
    fi
    echo 'map for' $LINKDEFNAME 'done'
  fi
fi

exit 0
