#!/bin/sh

echo "The script began at "
date

mkdir TestSCPs TestMATs TestEFFs TestDETs

for schno in 10 20 30 40 50
do
    stuno=10
    nameone=sch$schno
    nametwo=stu$stuno

    SCPfilename=$nameone$nametwo.scp
    MATfilename=$nameone$nametwo.mat
    EFFfilename=$nameone$nametwo.eff
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
    ../gcpsa ../TestSCPs/"$SCPfilename" > "$MATfilename"

    cd ../TestEFFs
    touch "$EFFfilename"
    ../effpardo ../TestMATs/"$MATfilename" ../TestSCPs/"$SCPfilename" 0 > "$EFFfilename"

    cd ../TestDETs
    touch "$DETfilename"
    ../purify ../TestEFFs/"$EFFfilename" > "$DETfilename"

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

if [ -d TestEFFs ]
then
    cd TestEFFs
    rm *
    cd ..
    rmdir TestEFFs
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
