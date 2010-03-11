#!/usr/bin/ksh
# Run GO4 analysis over LMD files of runs
# Files of a run are copied, processed and deleted
# first last run index 
first=$1
last=$2
# File prefix
run=t018f
# lmd data directory
TASCALMD=/d/ship02/tasca/t018
# local disk on lxg0708
TASCASTORE=/u/tasca/GO4_offline_t018/data/stepdata/checked03
# GO4 program
TASCAGO4=/u/tasca/GO4_offline_t018/checked01

. lealogin
. go4login 402-00

echo "======================================"
echo "Making directory files from $TASCALMD on $TASCASTORE ..."
echo "GO4 $TASCAGO4: Unpack, calibrate and check data on $TASCASTORE"
cd $TASCASTORE

# copy loop by lml files
ii=$first
while (($ii <= $last));do
 lmlfile=$(echo $run$(printf "%03d" $ii))
 files=$TASCALMD/$lmlfile
echo "======================================"
 cp $files*.lmd .
 lmlmake $run $ii $ii
cat $lmlfile.lml
# create the directory files
# time lmdirmake .

 cd $TASCAGO4
# Run the analysis over all lml files
#echo @$TASCASTORE/$lmlfile.lml
export TASCASTORE
 echo "------------------------------------"
time go4analysis -x -f @$TASCASTORE/$lmlfile.lml
cd $TASCASTORE
echo "rm $lmlfile.lml $lmlfile*.lmd"
 rm $lmlfile.lml $lmlfile*.lmd
 let ii=$ii+1
done


