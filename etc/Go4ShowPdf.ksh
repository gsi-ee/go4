#!/bin/bash

# simple script to activate any PDF viewer from gui

pdf=`which kfmclient`

if [[ -f $pdf ]] ; then 
   $pdf exec $1 &
   exit 0
fi

pdf=`which xpdf`

if [[ -f $pdf ]] ; then 
   $pdf $1 &
   exit 0
fi

pdf=`which acroread`

if [[ -f $pdf ]] ; then 
   $pdf $1 &
   exit 0
fi

pdf=`which kpdf`

if [[ -f $pdf ]] ; then 
   $pdf $1 &
   exit 0
fi

echo "Cannot found suitable pdf viewer"

exit 1   