#! /bin/bash

# script to generate dependency file
# dependency file itself depend from all sources and rebuild automatically

OBJSUF=$1
shift

trap "rm -f $1.tmp $1.tmp.bak; exit 1" 1 2 3 15

touch $1.tmp

rmkdepend -f$1.tmp -Y -o.$OBJSUF -w 30000 -- $2 -- $3 > /dev/null 2>&1
#makedepend -f$1.tmp -Y -w 30000 -- $2 -- $3 > /dev/null 2>&1
depstat=$?
if [ $depstat -ne 0 ]; then
   rm -f $1.tmp $1.tmp.bak
   exit $depstat
fi

if [ "$OBJSUF" = "obj" ]; then
sed -e 's@^\(.*\)\.obj:@\1.d \1.obj:@' -e 's@^#.*$@@' -e '/^$/d' $1.tmp
else
sed -e 's@^\(.*\)\.o:@\1.d \1.o:@' -e 's@^#.*$@@' -e '/^$/d' $1.tmp
fi

#sed -e 's@^\(.*\)\.o:@\1.d \1.o:@' -e 's@^#.*$@@' -e '/^$/d' $1.tmp
rm -f $1.tmp $1.tmp.bak

exit 0
