#!/bin/bash

RMMODULE=rootkit
INSMODULE=rootkit.ko

sudo rmmod $RMMODULE
sudo dmesg -c # Clear kernel log
make
sleep 1
sudo insmod $INSMODULE
sleep 1
sudo rmmod $RMMODULE
sleep 1
dmesg #Print kernel log
