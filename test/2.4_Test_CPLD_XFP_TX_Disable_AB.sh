source comm.sh
WaitForAnyKey "A1. Read XFP TX A "
../test_cpld -g -1 62  -2 15

WaitForAnyKey "A2. Set All(0) XFP TX A"
../test_cpld -s -1 62  -2 15  -3 00

WaitForAnyKey "A2.1 Read XFP TX A To Be 00 "
../test_cpld -g -1 62  -2 15

WaitForAnyKey "A3. Set All(1) XFP TX A"
../test_cpld -s -1 62  -2 15  -3 FF

WaitForAnyKey "A3.1 Read XFP TX A To Be FF "
../test_cpld -g -1 62  -2 15

WaitForAnyKey "B1. Read XFP TX B "
../test_cpld -g -1 62  -2 17

WaitForAnyKey "B2. Set All(0) XFP TX B"
../test_cpld -s -1 62  -2 17  -3 00

WaitForAnyKey "B2.1 Read XFP TX B To Be 00 "
../test_cpld -g -1 62  -2 17

WaitForAnyKey "B3. Set All(1) XFP TX B"
../test_cpld -s -1 62  -2 17  -3 FF

WaitForAnyKey "B3.1 Read XFP TX B To Be FF "
../test_cpld -g -1 62  -2 17