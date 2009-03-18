#! /bin/sh
# $1 - filename
# $2 - headername
HEAD=$2
if [ "x$HEAD" == "x" ]; then
   HEAD=header.txt
fi
echo pack $1 ...
cat $HEAD $1 blankline.txt >dummy.tmp
#this just copy file as it is
mv -f dummy.tmp $1
# this code to remove trailing tabs and spaces
#sed -e 's/[[:space:]]*$//g' <dummy.tmp >dummy2.tmp
#rm -f dummy.tmp
#mv -f dummy2.tmp $1
