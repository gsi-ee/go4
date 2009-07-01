#! /bin/sh

# Simple interface to LINK, tansforming -o <exe> to -out:<exe> and unix
# pathnames to windows pathnames.

args=
exename=

while [ "$1" != "" ]; do
   case "$1" in
   "-o") shift; args="$args -out:$1"; exename=$1 ;;
   *) args="$args $1" ;;
   esac
   shift
done

#/cygdrive/f/Serg/MVC/bin/link $args

echo "Producing $exename"

link $args

if [ $? != 0 ]; then exit $?; fi

echo "Processing $exename.manifest"

mt -nologo -manifest $exename.manifest -outputresource:$exename

rm -f $exename.manifest

exit $?
