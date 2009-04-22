#! /bin/sh

# Script to generate a shared library.
# Called by Makefile.

GO4_OS=$1

RM=$2
SOSUFFIX=$3
VESUFFIX=$4

LIBNAME=$5
LIBDIR=$6

if [ "x$LIBDIR" = "x" ]; then
   LIBDIR=.
fi

$RM $LIBDIR/$LIBNAME.$SOSUFFIX.$VESUFFIX
$RM $LIBDIR/$LIBNAME.$SOSUFFIX
$RM $LIBDIR/$LIBNAME.$SOSUFFIX.*
$RM $LIBDIR/$LIBNAME.rootmap
if [ "$GO4_OS" = "Win32" ]; then
  $RM $LIBDIR/$LIBNAME.*
  if [ "$LIBDIR" = "lib" ]; then
    $RM bin/$LIBNAME.dll
  fi
fi

exit 0

