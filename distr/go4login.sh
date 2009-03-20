#!/bin/bash
# note: replace first line by true location of bash on your linux!

# specify your OS : Linux (default), Win32, Solaris 
# export GO4_OS=Linux


# change next line to the true location of your root installation
export ROOTSYS=/home/user/root

#change next line to the location of your go4 system
export GO4SYS=/home/user/go4

# if QTDIR not yet specified, set correct location of qt installation
if [ "x$QTDIR" == "x" ]; then
   export QTDIR=/home/user/qt3
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$QTDIR/lib
   export PATH=$PATH:$QTDIR/bin
fi

export LD_LIBRARY_PATH=.:$GO4SYS/lib:$ROOTSYS/lib:$LD_LIBRARY_PATH
export PATH=$GO4SYS/bin:$ROOTSYS/bin:$PATH
