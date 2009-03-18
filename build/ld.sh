#! /bin/sh

# Simple interface to LINK, tansforming -o <exe> to -out:<exe> and unix
# pathnames to windows pathnames.

args=

while [ "$1" != "" ]; do
   case "$1" in
   "-o") shift; args="$args -out:$1" ;;
   *) args="$args $1" ;;
   esac
   shift
done

#/cygdrive/f/Serg/MVC/bin/link $args
link $args

exit $?
