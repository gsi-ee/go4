#! /bin/sh
# H.Essel 30.7.2002: three args
# usage: replace.sh file old new
FILELIST=$1
#echo $FILELIST
if [ $# -ne 3 ] ; then
echo 'args: file "oldstring" "newstring"'
else
echo Replace $2 by $3 in $1
for FILENAME in $FILELIST;
do
#echo $FILENAME;
cat  $FILENAME | awk -v rep=$3 -v str=$2 '{ gsub(str,rep) ; print }' > temp.txt
mv temp.txt $FILENAME;
NEWNAME=`echo $FILENAME | awk -v rep=$3 -v str=$2 '{ gsub(str,rep) ; print }'`;
echo $NEWNAME;
mv $FILENAME $NEWNAME;
done
fi

