#!/bin/sh

echo "The script began at "
date

mkdir SCPTests MATTests DETTests

for schno in 500
do
    stuno=200
    nameone=sch$schno
    nametwo=stu$stuno

    SCPfilename=$nameone$nametwo.scp
    MATfilename=$nameone$nametwo.mat
    DETfilename=$nameone$nametwo.det

    no_sch=$schno
    no_stu_per_sch=`expr $stuno - 1`
    sch_capacity=$stuno
    no_priority_grades=3

    cd SCPTests
    touch "$SCPfilename"
    ../makex $no_sch $no_stu_per_sch $sch_capacity $no_priority_grades > "$SCPfilename"

    cd ../MATTests
    touch "$MATfilename"
    ../emcc ../SCPTests/"$SCPfilename" > "$MATfilename"

    cd ../DETTests
    touch "$DETfilename"
    ../purify ../MATTests/"$MATfilename" > "$DETfilename"
    
    echo "We completed a computation at "
    date

    cd ..
done

if [ -d SCPTests ]
then
    cd SCPTests
    rm *
    cd ..
    rmdir SCPTests
fi

if [ -d MATTests ]
then
    cd MATTests
    rm *
    cd ..
    rmdir MATTests
fi

if [ -d DETTests ]
then
    cd DETTests
    rm *
    cd ..
    rmdir DETTests
fi

echo "The script ended at "
date
