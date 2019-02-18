source comm.sh
WaitForAnyKey "1.0 Read System Rest-2 "
../test_cpld -g -1 62  -2 37

WaitForAnyKey "2.0 Set All(0) System Rest-2 "
../test_cpld -s -1 62  -2 37 -3 00

WaitForAnyKey "2.1 Read System Rest-2 To Be 00 "
../test_cpld -g -1 62  -2 37

WaitForAnyKey "3.0 Set All(1) System Rest-2 "
../test_cpld -s -1 62  -2 37 -3 FF

WaitForAnyKey "3.1. Read System Rest-2 To Be FF "
../test_cpld -g -1 62  -2 37

