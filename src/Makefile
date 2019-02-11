obj-m+=asfvolt-cpld.o
obj-m+=asfvolt_dev.o
#KDIR := /lib/modules/$(shell uname -r)/build
KDIR := /home/voltha/Build-BAL2.6.1.3/OpenNetworkLinux/packages/base/amd64/kernels/kernel-4.14-lts-x86-64-all/builds/jessie/linux-4.14.49
PWD := $(shell pwd)

# The remaineder of this Makefile is similar to a regular Makefile
# The "$(shell uname -r)" is a useful call to return the current kernel build version
# The -C option switches the directory to the kernel direcotry before performing any make tasks
# The "M=$(PWD)" variable assignment tells the make command where the actual project files exist

# The "modules" target is the default target for external kernel modules.
# An alternative target is "modules_install", which would install the module (needs sudo)

all:
	make -C $(KDIR) M=$(PWD) modules
	$(CC) test_cpld.c -o test_cpld	
clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f test_cpld