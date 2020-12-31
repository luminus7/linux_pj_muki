#
# Makefile for the linux journaling routines.
#

obj-m += jbd3.o

jbd3-objs := transaction.o commit.o recovery.o checkpoint.o revoke.o journal.o



KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)


default :
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean :
	rm *.mod.*
	rm *.ko
	rm *.o
