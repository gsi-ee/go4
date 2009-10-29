#!/bin/bash
# $1 - filename
echo Processing $1 ...
# remove trailing tabs and spaces
sed 's/[[:space:]]*$//g' <$1 >dummy.txt
#replace tabs by three spaces
sed 's/[	]/   /g' <dummy.txt >dummy2.txt
diff $1 dummy2.txt >dummy.txt
SIZE=`ls -s dummy.txt | sed 's/ //g'`
#echo "aa$SIZE aa"
cat distr/header.txt dummy2.txt > $1
svn propset svn:keywords "Id" $1
rm -f dummy.txt
if [ "$SIZE" != "0dummy.txt" ]; then
  echo "File changed"
  #mv -f dummy2.txt $1
  rm -f dummy2.txt
else
  echo "File did not changed"
  rm -f dummy2.txt
fi
