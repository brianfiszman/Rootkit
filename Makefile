
obj-m+=rootkit.o
rootkit-objs += device-init.o
rootkit-objs += file-operations.o
rootkit-objs += main.o
 
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
