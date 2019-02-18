source comm.sh
count=0
while [1];
do
WaitForAnyKey "A1 Read System Rest-1 "
ipmitool raw 0x34 0x1E 0x36 

WaitForAnyKey "A2 Set All(0) System Rest-1 "
ipmitool raw 0x34 0x1F 0x36 0x00

WaitForAnyKey "A2.1 Read System Rest-1 To Be 00 "
ipmitool raw 0x34 0x1E 0x36


WaitForAnyKey "A3 Set All(1) System Rest-1 "
ipmitool raw 0x34 0x1F 0x36 0xFF

WaitForAnyKey "A3.1 Read System Rest-1 To Be FF"
ipmitool raw 0x34 0x1E 0x36

echo [$count]
count=$((count+1))

done
