#!/bin/bash
# -------------------------------------------------------
# Automatic preparation script for verify the CPLD Stress test
# JC Yu,     Sep. 05
# -------------------------------------------------------

sp=2
count=0
while [ 1 ];
do
#WaitForAnyKey "A1. Read XFP TX A "
../test_cpld -g -1 62  -2 15

#WaitForAnyKey "A2. Set All(0) XFP TX A"
sleep  0
../test_cpld -s -1 62  -2 15  -3 00

#WaitForAnyKey "A2.1 Read XFP TX A To Be 00 "
sleep  0
../test_cpld -g -1 62  -2 15

#WaitForAnyKey "A3. Set All(1) XFP TX A"
sleep  0
../test_cpld -s -1 62  -2 15  -3 FF

#WaitForAnyKey "A3.1 Read XFP TX A To Be FF "
sleep  0
../test_cpld -g -1 62  -2 15

#WaitForAnyKey "B1. Read XFP TX B "
sleep  0
../test_cpld -g -1 62  -2 17

#WaitForAnyKey "B2. Set All(0) XFP TX B"
sleep  0
../test_cpld -s -1 62  -2 17  -3 00

#WaitForAnyKey "B2.1 Read XFP TX B To Be 00 "
sleep  0
../test_cpld -g -1 62  -2 17

#WaitForAnyKey "B3. Set All(1) XFP TX B"
sleep  0
../test_cpld -s -1 62  -2 17  -3 FF

#WaitForAnyKey "B3.1 Read XFP TX B To Be FF "
sleep  0
../test_cpld -g -1 62  -2 17
echo [$count]
count=$(($count+1))
sleep 1
done
