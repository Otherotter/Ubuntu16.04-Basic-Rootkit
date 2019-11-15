# To run quick tests use the command "make test"
# https://stackoverflow.com/questions/20301591/m-option-in-make-command-makefile
#obj-m += quick.o
obj-m += rootkit.o

all:
	sudo make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

clean:
	sudo make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean

test:
	sudo dmesg -C # Clears the kernel ring buffer
	sudo insmod rootkit.ko # Insert our rootkit, run __init
	sudo rmmod rootkit.ko # Remove our rootkit, run __exit
	dmesg | grep -i "rootkit" # use "dmesg" and filter for our rootkit
