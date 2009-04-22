#! /bin/bash

# Script to create a shared library.
# Called by Makefile.

GO4_OS=$1

LD=$2
RM=$3
MV=$4
LN=$5
RLIBMAP=$6
ANALDEPLIBS=$7
LDFLAGS=$8
SOFLAGS=$9
SOSUFFIX=${10}
VESUFFIX=${11}

LIBNAME=${12}
LIBOBJS=${13}
LIBDIR=${14}

LINKDEF=${15}
DEPLIBS=${16}

# here is completely Win32 path

if [ "$GO4_OS" = "Win32" ]; then

  syslibs="msvcrt.lib oldnames.lib kernel32.lib advapi32.lib \
           user32.lib gdi32.lib comdlg32.lib winspool.lib \
           comctl32.lib WSock32.lib"
  
  rootlibs="libCore.lib libCint.lib libMatrix.lib \
            libHist.lib libGraf.lib libMinuit.lib \
            libGpad.lib libThread.lib libTree.lib libXMLIO.lib"
  
  if [[ -a $ROOTSYS/lib/libSpectrum.lib ]]; then
     rootlibs="$rootlibs libSpectrum.lib"
  fi
  
  if [[ -a $ROOTSYS/lib/libRIO.lib ]]; then
     rootlibs="$rootlibs libRIO.lib"
  fi
  
  if [[ -a $ROOTSYS/lib/libNet.lib ]]; then
     rootlibs="$rootlibs libNet.lib"
  fi
  
  extralibs=""
  isgo4lib="true"
  
  if [ "$LIBNAME" = "libGo4Fit" ]; then
    extralibs=""
  elif [ "$LIBNAME" = "libGo4Base" ]; then
    extralibs="libGo4Fit.lib"
  elif [ "$LIBNAME" = "libGo4ThreadManager" ]; then
    extralibs="libGo4Base.lib"
  elif [ "$LIBNAME" = "libGo4TaskHandler" ]; then
    extralibs="libGo4Base.lib libGo4ThreadManager.lib"
  elif [ "$LIBNAME" = "libGo4Version" ]; then
    extralibs=""
  elif [ "$LIBNAME" = "libGo4AnalBase" ]; then
    extralibs="libGo4Base.lib libGo4ThreadManager.lib libGo4TaskHandler.lib"
  elif [ "$LIBNAME" = "libGo4Analysis" ]; then
    extralibs="libGo4Fit.lib libGo4Base.lib libGo4ThreadManager.lib \
               libGo4TaskHandler.lib libGo4AnalBase.lib libGo4Version.lib"
  elif [ "$LIBNAME" = "libGo4GUI" ]; then
    extralibs="libGo4Fit.lib libGo4Base.lib libGo4ThreadManager.lib \
               libGo4TaskHandler.lib libGo4AnalBase.lib libGui.lib "
  else
  #this is a case for user libraries
  #add here more library if something is missing
    isgo4lib="false"
    extralibs="libGo4Fit.lib libGo4Base.lib libGo4ThreadManager.lib \
               libGo4TaskHandler.lib libGo4AnalBase.lib libGo4Analysis.lib"
  fi
  
  #name=`basename $LIBNAME.dll`
  #echo "name = $LIBNAME  extra = $extralibs"
  
  #bindexp=/cygdrive/f/Serg/root/bin/bindexplib
  bindexp=bindexplib
  
  rm -f $LIBDIR/$LIBNAME.*
  
  #echo $bindexp $LIBNAME $LIBOBJS > $LIBDIR/${LIBNAME}.def
  echo Produce $LIBDIR/${LIBNAME}.def
  
  $bindexp $LIBNAME $LIBOBJS > $LIBDIR/${LIBNAME}.def
  
  #echo /cygdrive/f/Serg/MVC/bin/link -LIB -nologo -MACHINE:IX86 \
  #      -out:$LIBDIR/${LIBNAME}.lib $LIBOBJS -def:$LIBDIR/${LIBNAME}.def
  
  link -LIB -nologo -MACHINE:IX86 \
        -out:$LIBDIR/${LIBNAME}.lib $LIBOBJS -def:$LIBDIR/${LIBNAME}.def
  
  #echo link $SOFLAGS $LDFLAGS -out:$LIBDIR/${LIBNAME}.dll $LIBOBJS \
  #      $LIBDIR/${LIBNAME}.exp $extralibs $rootlibs $syslibs
  
  echo Link $LIBDIR/${LIBNAME}.dll
  
  link $SOFLAGS $LDFLAGS -out:$LIBDIR/${LIBNAME}.dll $LIBOBJS \
     $LIBDIR/${LIBNAME}.exp $extralibs $rootlibs $syslibs
  
  cp $LIBDIR/${LIBNAME}.lib $LIBDIR/${LIBNAME}.lib.$VESUFFIX
  
  if [ "$LIBDIR" == "lib" ]; then
    if [ "$isgo4lib" = "true" ]; then
      echo Copy $LIBDIR/${LIBNAME}.dll to lib directory
      $MV $LIBDIR/${LIBNAME}.dll bin/${LIBNAME}.dll
      touch $LIBDIR/${LIBNAME}.dll
    fi
  fi
  
  exit 0

fi

# rest is Unix-like library building


if [ "x$LIBDIR" = "x" ]; then
   LIBDIR=.
fi

$RM $LIBDIR/$LIBNAME.$SOSUFFIX
$RM $LIBDIR/$LIBNAME.$SOSUFFIX.$VESUFFIX

if [ "$GO4_OS" = "Solaris" ]; then
   echo $LD $SOFLAGS $LDFLAGS $LIBOBJS -o $LIBNAME.$SOSUFFIX.$VESUFFIX

   $LD $SOFLAGS $LDFLAGS $LIBOBJS -o $LIBDIR/$LIBNAME.$SOSUFFIX.$VESUFFIX
else
   echo $LD $SOFLAGS$LIBNAME.$SOSUFFIX.$VESUFFIX $LDFLAGS $LIBOBJS -o $LIBNAME.$SOSUFFIX.$VESUFFIX

   $LD $SOFLAGS$LIBNAME.$SOSUFFIX.$VESUFFIX $LDFLAGS $LIBOBJS \
            -o $LIBDIR/$LIBNAME.$SOSUFFIX.$VESUFFIX
fi

CURDIR=`pwd`

cd $LIBDIR; $LN $LIBNAME.$SOSUFFIX.$VESUFFIX $LIBNAME.$SOSUFFIX

echo $LIBNAME.$SOSUFFIX 'done'

if [ "x$LINKDEF" != "x" ]; then
   if [ "x$DEPLIBS" = "x" ]; then
      DEPLIBS=$ANALDEPLIBS
   fi
   cd $CURDIR
   echo $RLIBMAP -r $LIBDIR/$LIBNAME.rootmap -l $LIBDIR/$LIBNAME.$SOSUFFIX -d $DEPLIBS -c $LINKDEF
   $RLIBMAP -r $LIBDIR/$LIBNAME.rootmap -l $LIBDIR/$LIBNAME.$SOSUFFIX -d $DEPLIBS -c $LINKDEF
   echo $LIBNAME.rootmap 'done'
fi
   
exit 0
