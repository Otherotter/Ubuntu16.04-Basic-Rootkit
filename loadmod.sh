#!/bin/bash

RMMODULE=rootkit
INSMODULE=rootkit.ko

sudo rmmod $RMMODULE
#sudo rmmod quick
make
sleep 1
sudo insmod $INSMODULE
sudo insmod quick.ko
sleep 1
sudo rmmod $RMMODULE
#sudo rmmod quick
sleep 1
tail /var/log/kern.log
