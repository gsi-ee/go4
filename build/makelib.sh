#! /bin/bash

# Script to create a shared library.
# Called by Makefile.

GO4_OS=$1

LD=$2
RM=$3
MV=$4
LN=$5
RLIBMAP=$6
LDFLAGS=$7
SOFLAGS=$8
SOSUFFIX=$9
VESUFFIX=${10}

LIBNAME=${11}
LIBOBJS=${12}
LIBDIR=${13}

LINKDEF=${14}
DEPLIBS=${15}

# here is completely Win32 path

if [ "$GO4_OS" = "Win32" ]; then

  ROOTLIBDIR=`root-config --libdir`

  syslibs="ws2_32.lib"

  rootlibs="libCore.lib libCint.lib libMatrix.lib \
            libHist.lib libGraf.lib libMinuit.lib libMathCore.lib \
            libGpad.lib libThread.lib libTree.lib libXMLIO.lib"
  
  if [[ -a $ROOTLIBDIR/libSpectrum.lib ]]; then
     rootlibs="$rootlibs libSpectrum.lib"
  fi
  
  if [[ -a $ROOTLIBDIR/libRIO.lib ]]; then
     rootlibs="$rootlibs libRIO.lib"
  fi
  
  if [[ -a $ROOTLIBDIR/libNet.lib ]]; then
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
  elif [ "$LIBNAME" = "libGo4AnalBase" ]; then
    extralibs="libGo4Base.lib libGo4ThreadManager.lib libGo4TaskHandler.lib"
  elif [ "$LIBNAME" = "libGo4Analysis" ]; then
    extralibs="libGo4Fit.lib libGo4Base.lib libGo4ThreadManager.lib \
               libGo4TaskHandler.lib libGo4AnalBase.lib"
  elif [ "$LIBNAME" = "libGo4GUI" ]; then
    extralibs="libGo4Fit.lib libGo4Base.lib libGo4ThreadManager.lib \
               libGo4TaskHandler.lib libGo4AnalBase.lib libGui.lib"
  else
  #this is a case for user libraries
  #add here more library if something is missing
    isgo4lib="false"
    extralibs="libGo4Fit.lib libGo4Base.lib libGo4ThreadManager.lib \
               libGo4TaskHandler.lib libGo4AnalBase.lib libGo4Analysis.lib"
  fi
  
  rm -f $LIBDIR/$LIBNAME.*
  
  #echo bindexplib $LIBNAME $LIBOBJS > $LIBDIR/${LIBNAME}.def
  echo Produce $LIBDIR/${LIBNAME}.def
  
  bindexplib $LIBNAME $LIBOBJS > $LIBDIR/${LIBNAME}.def
  
  #echo /cygdrive/f/Serg/MVC/bin/link -LIB -nologo -MACHINE:IX86 \
  #      -out:$LIBDIR/${LIBNAME}.lib $LIBOBJS -def:$LIBDIR/${LIBNAME}.def
  
  link -LIB -nologo -MACHINE:IX86 \
        -out:$LIBDIR/${LIBNAME}.lib $LIBOBJS -def:$LIBDIR/${LIBNAME}.def
  
  #echo link $SOFLAGS $LDFLAGS -out:$LIBDIR/${LIBNAME}.dll $LIBOBJS \
  #      $LIBDIR/${LIBNAME}.exp $extralibs $rootlibs $syslibs
  
  echo Link $LIBDIR/${LIBNAME}.dll
  
  link $SOFLAGS $LDFLAGS -out:$LIBDIR/${LIBNAME}.dll $LIBOBJS \
     $LIBDIR/${LIBNAME}.exp $extralibs $rootlibs $syslibs

  mt -nologo -manifest $LIBDIR/${LIBNAME}.dll.manifest -outputresource:$LIBDIR/${LIBNAME}.dll

  rm -f $LIBDIR/${LIBNAME}.dll.manifest

  if [ "x$LINKDEF" != "x" ]; then
     echo $RLIBMAP -r $LIBDIR/$LIBNAME.rootmap -l $LIBDIR/$LIBNAME.dll -d ${DEPLIBS//.lib/.dll} -c $LINKDEF
     $RLIBMAP -r $LIBDIR/$LIBNAME.rootmap -l $LIBDIR/$LIBNAME.dll -d ${DEPLIBS//.lib/.dll} -c $LINKDEF
     echo $LIBNAME.rootmap 'done'
  fi
  
  if [ "$LIBDIR" == "lib" ]; then
    if [ "$isgo4lib" == "true" ]; then
      echo Copy $LIBDIR/${LIBNAME}.dll to bin directory
      $MV $LIBDIR/${LIBNAME}.dll bin/${LIBNAME}.dll
      if [ "x$LINKDEF" != "x" ]; then
         cp $LIBDIR/${LIBNAME}.rootmap bin/${LIBNAME}.rootmap
      fi
    fi
  fi
  
  exit 0

fi

# rest is Unix-like library building


if [ "x$LIBDIR" = "x" ]; then
   LIBDIR=.
fi

if [ "$VESUFFIX" = "---" ]; then
   FULLSUFIX=$SOSUFFIX
else
   FULLSUFIX=$SOSUFFIX.$VESUFFIX
   $RM $LIBDIR/$LIBNAME.$SOSUFFIX
fi

$RM $LIBDIR/$LIBNAME.$FULLSUFIX

if [ "$GO4_OS" = "Solaris" ]; then
   echo $LD $SOFLAGS $LDFLAGS $LIBOBJS -o $LIBNAME.$FULLSUFIX

   $LD $SOFLAGS $LDFLAGS $LIBOBJS -o $LIBDIR/$LIBNAME.$FULLSUFIX
elif [ "$GO4_OS" = "Darwin" ]; then
echo $LD $SOFLAGS$LIBNAME.$FULLSUFIX $LDFLAGS $LIBOBJS -o $LIBNAME.$FULLSUFIX

   $LD $SOFLAGS$LIBNAME.$FULLSUFIX $LDFLAGS $LIBOBJS \
            -o $LIBDIR/$LIBNAME.$FULLSUFIX
if [ "$SOSUFFIX" = "dylib" ]; then
   $LN $LIBDIR/$LIBNAME.$FULLSUFIX $LIBDIR/$LIBNAME.so
fi

else
   echo $LD $SOFLAGS$LIBNAME.$FULLSUFIX $LDFLAGS $LIBOBJS -o $LIBNAME.$FULLSUFIX

   $LD $SOFLAGS$LIBNAME.$FULLSUFIX $LDFLAGS $LIBOBJS \
            -o $LIBDIR/$LIBNAME.$FULLSUFIX
fi

CURDIR=`pwd`

if [ "$VESUFFIX" != "---" ]; then
   cd $LIBDIR; $LN $LIBNAME.$SOSUFFIX.$VESUFFIX $LIBNAME.$SOSUFFIX
fi

echo $LIBNAME.$SOSUFFIX 'done'

if [ "x$LINKDEF" != "x" ]; then
   cd $CURDIR
   echo $RLIBMAP -r $LIBDIR/$LIBNAME.rootmap -l $LIBDIR/$LIBNAME.$SOSUFFIX -d $DEPLIBS -c $LINKDEF
   $RLIBMAP -r $LIBDIR/$LIBNAME.rootmap -l $LIBDIR/$LIBNAME.$SOSUFFIX -d $DEPLIBS -c $LINKDEF
   echo $LIBNAME.rootmap 'done'
fi
   
exit 0
