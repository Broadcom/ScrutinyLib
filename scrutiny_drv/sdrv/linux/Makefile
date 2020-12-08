
#
# SliffDriver - Compilation script
# 
CONFIG_MODULE_SIG=n
CONFIG_MODULE_SIG_ALL=n

obj-m = SliffDriver.o

KVERSION = $(shell uname -r)

all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean

