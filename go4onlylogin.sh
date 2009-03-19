#!/bin/bash

# specify your OS : Linux, Win32, Solaris 
# export GO4_OS=Linux

export GO4SYS=`pwd`

LD_LIBRARY_PATH=.:$GO4SYS/lib:$LD_LIBRARY_PATH
PATH=$GO4SYS/bin:$PATH
export LD_LIBRARY_PATH PATH
