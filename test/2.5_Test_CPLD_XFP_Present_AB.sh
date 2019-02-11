source comm.sh
WaitForAnyKey "A1. Read XFP Present A "
${CPLD_CMD} -g -1 62  -2 05

WaitForAnyKey "A2. Set All(0) XFP Present A--Should not work fine !!"
${CPLD_CMD} -s -1 62  -2 05  -3 00

WaitForAnyKey "A3. Set All(1) XFP Present A--Should not work fine !!"
${CPLD_CMD} -s -1 62  -2 05  -3 FF

WaitForAnyKey "B1. Read XFP Present B "
${CPLD_CMD} -g -1 62  -2 07

WaitForAnyKey "A2. Set All(0) XFP Present B--Should not work fine !!"
${CPLD_CMD} -s -1 62  -2 07  -3 00

WaitForAnyKey "A3. Set All(1) XFP Present B--Should not work fine !!"
${CPLD_CMD} -s -1 62  -2 07  -3 FF
