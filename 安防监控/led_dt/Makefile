DEV_NAME = fsled
#APP_NAME = test_beeper

ifeq ($(KERNELRELEASE),)  

ifeq ($(ARCH),arm)  
CROSS_COMPILE = arm-none-linux-gnueabi-
CC = $(CROSS_COMPILE)gcc
KERNELDIR ?= /home/legendfantasy/fs4412/linux-3.14  
ROOTFS ?= /opt/4412/rootfs  
else
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
endif  

PWD := $(shell pwd)  

modules:  
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules 
#$(CC) $(APP_NAME).c -o $(APP_NAME)
install:
	cp -raf  $(DEV_NAME).ko /opt/4412/rootfs/drv/
#cp -raf  $(APP_NAME) /opt/4412/rootfs/drv/

clean:
	rm -rf  *.o  *.ko  .*.cmd  *.mod.*  modules.order  Module.symvers   .tmp_versions 
#rm -rf $(APP_NAME)

else
obj-m  += $(DEV_NAME).o


endif

