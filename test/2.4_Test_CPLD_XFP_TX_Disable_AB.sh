source comm.sh
WaitForAnyKey "A1. Read XFP TX A "
${CPLD_CMD} -g -1 62  -2 15

WaitForAnyKey "A2. Set All(0) XFP TX A"
${CPLD_CMD} -s -1 62  -2 15  -3 00

WaitForAnyKey "A3. Set All(1) XFP TX A"
${CPLD_CMD} -s -1 62  -2 15  -3 FF

WaitForAnyKey "B1. Read XFP TX B "
${CPLD_CMD} -g -1 62  -2 17

WaitForAnyKey "B2. Set All(0) XFP TX B"
${CPLD_CMD} -s -1 62  -2 17  -3 00

WaitForAnyKey "B3. Set All(1) XFP TX B"
${CPLD_CMD} -s -1 62  -2 17  -3 FF
