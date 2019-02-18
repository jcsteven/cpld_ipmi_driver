#!/bin/bash
# -------------------------------------------------------
# Automatic preparation script for verify the CPLD Stress test
# JC Yu,     Sep. 05
# -------------------------------------------------------

sp=2
count=0
while [ 1 ];
do
#WaitForAnyKey "1. Test for CPLD Verion Read"
../test_cpld -g -1 62  -2 02
echo [$count]
count=$(($count+1))
sleep 1
done
