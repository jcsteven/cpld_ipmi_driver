source comm.sh
WaitForAnyKey "A1. Read XFP Present A "
../test_cpld  -g -1 62  -2 05

WaitForAnyKey "A2. Set All(0) XFP Present A--Should not work fine !!"
../test_cpld -s -1 62  -2 05  -3 00

WaitForAnyKey "A2.1 Read XFP Present A-To Be Not Change "
../test_cpld -g -1 62  -2 05


WaitForAnyKey "A3. Set All(1) XFP Present A--Should not work fine !!"
../test_cpld -s -1 62  -2 05  -3 FF

WaitForAnyKey "A3.1 Read XFP Present A-To Be Not Change "
../test_cpld -g -1 62  -2 05

WaitForAnyKey "B1. Read XFP Present B "
../test_cpld -g -1 62  -2 07

WaitForAnyKey "B2. Set All(0) XFP Present B--Should not work fine !!"
../test_cpld -s -1 62  -2 07  -3 00

WaitForAnyKey "B2.1 Read XFP Present B-To Be Not Change  "
../test_cpld -g -1 62  -2 07

WaitForAnyKey "B3. Set All(1) XFP Present B--Should not work fine !!"
../test_cpld -s -1 62  -2 07  -3 FF

WaitForAnyKey "B3.1 Read XFP Present B-To Be Not Change  "
../test_cpld -g -1 62  -2 07