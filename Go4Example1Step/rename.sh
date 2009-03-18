#! /usr/local/bin/bash
# Replace Go4 demo analysis by user experiment names
# J.Adamczewski 18.03.2002
# H.Essel 30.7.2002: two args
# The GSI Online Offline Object Oriented (GO4) Project
# Experiment Data Processing at DVEE department, GSI
# usage: rename.sh old new
FILELIST='Makefile Module.mk *.cxx *.h *.C Readme.txt'
#echo $FILELIST
if [ $# -ne 2 ] ; then
echo 'args: "oldstring" "newstring"'
else
echo Replace $1 by $2 in
echo "Makefile Module.mk *.cxx *.cpp *.h Readme.txt"
for FILENAME in $FILELIST;
do
#echo $FILENAME;
cat  $FILENAME | awk -v rep=$2 -v str=$1 '{ gsub(str,rep) ; print }' > temp.txt
mv temp.txt $FILENAME;
NEWNAME=`echo $FILENAME | awk -v rep=$2 -v str=$1 '{ gsub(str,rep) ; print }'`;
echo $NEWNAME;
mv $FILENAME $NEWNAME;
done
fi

