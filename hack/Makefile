KERNELDIR = /lib/modules/$(shell uname -r)/build

obj-m = hack.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.kr
	rm -rf *.mod.*
	rm -rf .*.cmd
	rm -rf *.o
