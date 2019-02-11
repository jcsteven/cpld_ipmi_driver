source comm.sh
WaitForAnyKey "A1. Read PON POTR A "
${CPLD_CMD} -g -1 62  -2 44

WaitForAnyKey "A2. Set All(0)PON POTR A"
${CPLD_CMD} -s -1 62  -2 44  -3 00

WaitForAnyKey "A3. Set All(1)PON POTR A"
${CPLD_CMD} -s -1 62  -2 44  -3 FF

WaitForAnyKey "B1. Read PON POTR B "
${CPLD_CMD} -g -1 62  -2 45

WaitForAnyKey "A2. Set All(0)PON POTR B"
${CPLD_CMD} -s -1 62  -2 45  -3 00

WaitForAnyKey "A2. Set All(1)PON POTR B"
${CPLD_CMD} -s -1 62  -2 45  -3 FF

