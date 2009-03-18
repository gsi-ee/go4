#! /bin/sh

# Script to generate a shared library.
# Called by Makefile.

GO4_OS=$1

SOSUFFIX=$2
VESUFFIX=$3

LIBNAME=$4
LIBDIR=$5

if [ "x$LIBDIR" != "x" ]; then
   rm -f $LIBDIR/$LIBNAME.$SOSUFFIX.$VESUFFIX
   rm -f $LIBDIR/$LIBNAME.$SOSUFFIX
   rm -f $LIBDIR/$LIBNAME.$SOSUFFIX.*
   if [ "$GO4_OS" = "Win32" ]; then
      rm -f $LIBDIR/$LIBNAME.*
      if [ "$LIBDIR" = "lib" ]; then
         rm -f bin/$LIBNAME.dll
      fi
   fi
else
   rm -f $LIBNAME.$SOSUFFIX.$VESUFFIX
   rm -f $LIBNAME.$SOSUFFIX
   rm -f $LIBNAME.$SOSUFFIX.*
   if [ "$GO4_OS" = "Win32" ]; then
      rm -f $LIBNAME.*
   fi
fi

exit 0

