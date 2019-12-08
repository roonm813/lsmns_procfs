MYPROC=procfs
obj-m += $(MYPROC).o 

export KROOT=/lib/modules/$(shell uname -r)/build 

all: 
	make -C $(KROOT) M=$(PWD) modules 
clean: 
	make -C $(KROOT) M=$(PWD) clean
