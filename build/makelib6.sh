#!/bin/bash

# Script to create a shared library for ROOT6.
# Called by Makefile.

GO4_OS=${1}

LD=${2}
RM=${3}
MV=${4}
LN=${5}
LDFLAGS=${6}
SOFLAGS=${7}
SOSUFFIX=${8}
ROOTEXEPATH=${9}
MAKELIB_SET=${10}
CXXOPTIONS=${11}

LIBNAME=${12}
LIBOBJS=${13}
LIBDIR=${14}

LINKDEF=${15}
DEPLIBS=${16}

# these arguments were introduced for ROOT6 and was not exists before
# if these arguments are not provided, the only way to guess them
DICTIONARY=${17}
HEADERS=${18}

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

  link /MANIFEST $SOFLAGS $LDFLAGS -out:$LIBDIR/${LIBNAME}.dll $LIBOBJS \
     $LIBDIR/${LIBNAME}.exp $extralibs $rootlibs $syslibs

  echo Process manifest $LIBDIR/${LIBNAME}.dll.manifest

  mt -nologo -manifest $LIBDIR/${LIBNAME}.dll.manifest -outputresource:$LIBDIR/${LIBNAME}.dll

  rm -f $LIBDIR/${LIBNAME}.dll.manifest

#  if [ "x$LINKDEF" != "x" ]; then
#     echo $RLIBMAP -r $LIBDIR/$LIBNAME.rootmap -l $LIBDIR/$LIBNAME.dll -d ${DEPLIBS//.lib/.dll} -c $LINKDEF
#     $RLIBMAP -r $LIBDIR/$LIBNAME.rootmap -l $LIBDIR/$LIBNAME.dll -d ${DEPLIBS//.lib/.dll} -c $LINKDEF
#     echo $LIBNAME.rootmap 'done'
#  fi
  
  if [[ "x$LINKDEF" != "x" ]]; then
    if [[ "x$DICTIONARY" != "x" && "x$HEADERS" != "x" ]]; then
       rootcling -r $DICTIONARY -rml $LIBNAME.$SOSUFFIX -rmf $LIBDIR/$LIBNAME.rootmap -s $LIBDIR/$LIBNAME.$SOSUFFIX $CXXOPTIONS $HEADERS $LINKDEF
       echo $LIBNAME.rootmap done
    else
       echo "Not enough information to produce $LIBNAME.rootmap file"
       echo "Please update your Makefile" 
    fi
  fi
  
  if [[ "$LIBDIR" == "lib" && "$isgo4lib" == "true" ]]; then
      echo Copy $LIBDIR/${LIBNAME}.dll to bin directory
      $MV $LIBDIR/${LIBNAME}.dll bin/${LIBNAME}.dll
      if [ "x$LINKDEF" != "x" ]; then
         cp $LIBDIR/${LIBNAME}.rootmap bin/${LIBNAME}.rootmap
      fi
  fi
  
  exit 0

fi

# rest is Unix-like library building


if [ "x$LIBDIR" = "x" ]; then
   LIBDIR=.
fi

FULLSUFIX=$SOSUFFIX

$RM $LIBDIR/$LIBNAME.$FULLSUFIX

if [ "$GO4_OS" = "Solaris" ]; then
   echo $LD $SOFLAGS $LDFLAGS $LIBOBJS $MAKELIB_SET -o $LIBNAME.$FULLSUFIX

   $LD $SOFLAGS $LDFLAGS $LIBOBJS $MAKELIB_SET -o $LIBDIR/$LIBNAME.$FULLSUFIX

elif [ "$GO4_OS" = "Darwin" ]; then
   echo $LD $SOFLAGS$LIBNAME.$FULLSUFIX $LDFLAGS $LIBOBJS -o $LIBNAME.$FULLSUFIX $MAKELIB_SET 

   $LD $SOFLAGS$LIBNAME.$FULLSUFIX $LDFLAGS $LIBOBJS -o $LIBDIR/$LIBNAME.$FULLSUFIX $MAKELIB_SET 

   if [ "$SOSUFFIX" = "dylib" ]; then
      $LN $LIBDIR/$LIBNAME.$FULLSUFIX $LIBDIR/$LIBNAME.so
   fi
else
   echo Linking $LIBNAME.$SOSUFFIX
   
   echo $LD $SOFLAGS$LIBNAME.$FULLSUFIX $LDFLAGS $LIBOBJS -o $LIBDIR/$LIBNAME.$FULLSUFIX $MAKELIB_SET

   $LD $SOFLAGS$LIBNAME.$FULLSUFIX $LDFLAGS $LIBOBJS -o $LIBDIR/$LIBNAME.$FULLSUFIX $MAKELIB_SET
fi

retval=$?
if [ $retval -ne 0 ]; then
   echo Fail to build $LIBNAME.$SOSUFFIX
   exit $retval
fi

echo $LIBNAME.$SOSUFFIX done

if [[ "x$LINKDEF" != "x" ]]; then
   if [[ "x$DICTIONARY" != "x" && "x$HEADERS" != "x" ]]; then
      $ROOTEXEPATH/bin/rootcling -r $DICTIONARY -rml $LIBNAME.$SOSUFFIX -rmf $LIBDIR/$LIBNAME.rootmap -s $LIBDIR/$LIBNAME.$SOSUFFIX $CXXOPTIONS $HEADERS $LINKDEF
      echo $LIBNAME.rootmap done
   else
      echo "Not enough information to produce $LIBNAME.rootmap file"
      echo "Please update your Makefile" 
   fi
fi

exit 0
