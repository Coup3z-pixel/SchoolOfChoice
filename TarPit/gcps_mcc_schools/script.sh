#!/bin/sh

echo "The script began at "
date

mkdir TestSCPs TestMATs TestDETs

for schno in 10 15 20 25 30 35 40 45
do
    stuno=5
    nameone=sch$schno
    nametwo=stu$stuno

    SCPfilename=$nameone$nametwo.scp
    MATfilename=$nameone$nametwo.mat
    DETfilename=$nameone$nametwo.det

    no_sch=$schno
    no_stu_per_sch=`expr $stuno - 1`
    sch_capacity=$stuno
    no_priority_grades=3

    cd TestSCPs
    touch "$SCPfilename"
    ../makex $no_sch $no_stu_per_sch $sch_capacity $no_priority_grades > "$SCPfilename"

    cd ../TestMATs
    touch "$MATfilename"
    ../gcps ../TestSCPs/"$SCPfilename" > "$MATfilename"

    cd ../TestDETs
    touch "$DETfilename"
    ../purify ../TestMATs/"$MATfilename" > "$DETfilename"

    cd ..
done

if [ -d TestSCPs ]
then
    cd TestSCPs
    rm *
    cd ..
    rmdir TestSCPs
fi

if [ -d TestMATs ]
then
    cd TestMATs
    rm *
    cd ..
    rmdir TestMATs
fi

if [ -d TestDETs ]
then
    cd TestDETs
    rm *
    cd ..
    rmdir TestDETs
fi

echo "The script ended at "
date
