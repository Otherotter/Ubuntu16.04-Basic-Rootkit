# To run quick tests use the command "make test"
obj-m += rootkit.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

test:
 sudo dmesg -C # Clears the kernel ring buffer
 sudo insmod rootkit.ko # Insert our rootkit, run __init
 sudo rmmod rootkit.ko # Remove our rootkit, run __exit
 dmesg | grep -i "rootkit" # use "dmesg" and filter for our rootkit
