#!/bin/bash
# -------------------------------------------------------
# Automatic preparation script for verify the CPLD Stress test
# JC Yu,     Sep. 05
# -------------------------------------------------------

sp=2
count=0
while [ 1 ];
do
#WaitForAnyKey "1.0 Read System Rest-2 "
../test_cpld -g -1 62  -2 37

#WaitForAnyKey "2.0 Set All(0) System Rest-2 "
sleep  0
../test_cpld -s -1 62  -2 37 -3 00

#WaitForAnyKey "2.1 Read System Rest-2 To Be 00 "
sleep  0
../test_cpld -g -1 62  -2 37

#WaitForAnyKey "3.0 Set All(1) System Rest-2 "
sleep  0
../test_cpld -s -1 62  -2 37 -3 FF

#WaitForAnyKey "3.1. Read System Rest-2 To Be FF "
sleep  0
../test_cpld -g -1 62  -2 37
echo [$count]
count=$(($count+1))
sleep 1
done
