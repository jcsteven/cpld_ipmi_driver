source comm.sh
WaitForAnyKey "A1. Read PON POTR A "
../test_cpld -g -1 62  -2 44

WaitForAnyKey "A2. Set All(0)PON POTR A"
../test_cpld -s -1 62  -2 44  -3 00

WaitForAnyKey "A2.1 Read PON POTR A--To Be 00 "
../test_cpld -g -1 62  -2 44

WaitForAnyKey "A3. Set All(1)PON POTR A"
../test_cpld -s -1 62  -2 44  -3 FF

WaitForAnyKey "A3.1 Read PON POTR A--To Be FF "
../test_cpld -g -1 62  -2 44

WaitForAnyKey "B1. Read PON POTR B "
../test_cpld -g -1 62  -2 45

WaitForAnyKey "B2. Set All(0)PON POTR B"
../test_cpld -s -1 62  -2 45  -3 00

WaitForAnyKey "B2.1 Read PON POTR B--To Be 00 "
../test_cpld -g -1 62  -2 45

WaitForAnyKey "B3. Set All(1)PON POTR B"
../test_cpld -s -1 62  -2 45  -3 FF

WaitForAnyKey "B3.1 Read PON POTR B--To Be FF "
../test_cpld -g -1 62  -2 45

