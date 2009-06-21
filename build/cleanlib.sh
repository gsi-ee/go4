#! /bin/sh

# Script to generate a shared library.
# Called by Makefile.

GO4_OS=$1

RM=$2
SOSUFFIX=$3

LIBNAME=$4
LIBDIR=$5

if [ "x$LIBDIR" = "x" ]; then
   LIBDIR=.
fi

$RM $LIBDIR/$LIBNAME.$SOSUFFIX
$RM $LIBDIR/$LIBNAME.$SOSUFFIX.*
$RM $LIBDIR/$LIBNAME.rootmap
if [ "$GO4_OS" = "Win32" ]; then
  $RM $LIBDIR/$LIBNAME.*
  if [ "$LIBDIR" = "lib" ]; then
    $RM bin/$LIBNAME.dll
    $RM bin/$LIBNAME.rootmap
  fi
fi

exit 0

