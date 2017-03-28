obj-m := hello.o ro_file.o param.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	rm -f *.o *.ko *.mod.c Module.symvers modules.order
