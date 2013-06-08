# If KERNELRELEASE is defined, we've been invoked from the

# kernel build system and can use its language.

	obj-m := goldengate.o
	goldengate-objs := golden.o goldenchar.o goldenblock.o utils.o

KERNELDIR ?= /home/lamer/projects/linux-3.8.5

PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
