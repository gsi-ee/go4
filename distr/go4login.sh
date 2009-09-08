#!/bin/bash
# note: replace first line by true location of bash on your linux!

# specify your OS : Linux (default), Win32, Solaris 
# export GO4_OS=Linux

# uncomment following lines, if ROOT installed in non-default location
# export ROOTSYS=/home/user/root
# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib
# export PATH=$PATH:$ROOTSYS/bin

# uncomment following lines, if Qt installed in non-default location
# export QTDIR=/home/user/qt4
# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$QTDIR/lib
# export PATH=$PATH:$QTDIR/bin

#change next line to the location of your go4 system
export GO4SYS=`pwd`
export LD_LIBRARY_PATH=.:$GO4SYS/lib:$LD_LIBRARY_PATH
export PATH=$PATH:$GO4SYS/bin

