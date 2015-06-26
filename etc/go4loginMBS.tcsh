#!/bin/tcsh 
# Go4 login script.
# Modified 9.Nov.2009 /HE
# Second argument for qt3 or qt4
# 8.Feb 2010 /JAM
# Adjustments for debian50 /lenny
# 27.Nov 2013 adjusted for debian7/wheezy JAM
# 5 Dec 2014 ported to MBS Debian nodes JAM
# 8 Dec 2014 try to put it under tcsh  JAM
# 26 Jun 2015 clean up and extend tcsh version for version argument JAM
echo Go4 login for MBS Linux with tcsh...

[ -z "$GSI_OS_TYPE" ] && export GSI_OS_TYPE=Deb
[ -z "$GSI_OS_VERSION" ] && export GSI_OS_VERSION=3.2
[ -z "$GSI_CPU_PLATFORM" ] && export GSI_CPU_PLATFORM=PCx86

#CPUKIND=$(uname -m)
setenv VERSIONPATH /analysis/${GSI_CPU_PLATFORM}_${GSI_OS}_${GSI_OS_VERSION}_${GSI_OS_TYPE}
setenv GO4PATH $VERSIONPATH/go4
setenv DABCPATH /dabc/${GSI_CPU_PLATFORM}_${GSI_OS}_${GSI_OS_VERSION}_${GSI_OS_TYPE}

setenv GO4DIR 500-00
setenv GO4VERSION v5.0.0

if ( ($#) > 0) then
  setenv GO4DIR $1
  setenv GO4VERSION $1
  shift
endif

set AVAIL_GO4_VERSIONS=`ls $GO4PATH`


if ( $GO4DIR == "-h" || $GO4DIR == "--help" ) then
   echo ''
   echo '----------------------------------------------------------------------'
   echo  This is the MBS Linux go4login help
   echo  Installed Go4 versions at $GO4PATH :
   foreach DIRNAME ($AVAIL_GO4_VERSIONS)
	echo $DIRNAME
   end
   echo ''
   echo type go4login XXX-XX with desired version name 
   echo '----------------------------------------------------------------------'
   exit
endif


if ( ! -d $GO4PATH/$GO4DIR )  then
  echo "Requested Go4 version:  $GO4DIR   is not available"
  echo Select from: $AVAIL_GO4_VERSIONS
  exit 
endif

# clean up previous go4login parts from  path and ldpath:
set OLD_PATH
set PATHLIST = `echo $PATH | sed 's/:/ /g'`
foreach  pdir ($PATHLIST)
    set PFOUND = `echo $pdir | grep $VERSIONPATH`
    set PDABCFOUND = `echo $pdir | grep $DABCPATH`
    if ( ("x$PFOUND" == "x") && ("x$PDABCFOUND" == "x") ) then 
	set OLD_PATH = $OLD_PATH':'$pdir
    endif
end
setenv PATH $OLD_PATH


set OLD_LDPATH
set LDPATHLIST = `echo $LD_LIBRARY_PATH | sed 's/:/ /g'`
foreach  ldir ($LDPATHLIST)  
    if ( "$ldir" != "." ) then
       set LFOUND = `echo $ldir | grep $VERSIONPATH`
       set LDABCFOUND = `echo $ldir | grep $DABCPATH`
       if ( ("x$LFOUND" == "x") && ("x$LDABCFOUND" == "x") )  then 
         set OLD_LDPATH = $OLD_LDPATH':'$ldir
       endif
    endif
end
setenv LD_LIBRARY_PATH $OLD_LDPATH

#echo cleaned path: $PATH
#echo cleaned ldpath: $LD_LIBRARY_PATH


# now activate go4login, will set dabc and root correctly:
source  $GO4PATH/$GO4DIR/go4loginsave.tcsh

echo "Go4  version $GO4VERSION enabled, path $GO4SYS !"
