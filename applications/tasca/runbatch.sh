#!/usr/bin/ksh
#
# first last index 
first=$1
last=$2
run=t018f

. lealogin
. go4login 402-00

# lmd data directory
TASCALMD=/d/ship02/tasca/t018
# local disk on lxg0708
TASCASTORE=/u/tasca/GO4_offline_t018/data/stepdata/checked03
TASCAGO4=/u/tasca/GO4_offline_t018/checked01
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
# create the directory files
# time lmdirmake .

 cd $TASCAGO4
# Run the analysis over all lml files
#echo @$TASCASTORE/$lmlfile.lml
export TASCASTORE
 echo "------------------------------------"
time ./MainUserAnalysis -f @$TASCASTORE/$lmlfile.lml
cd $TASCASTORE
 rm *.lml *.lmd
 let ii=$ii+1
done


