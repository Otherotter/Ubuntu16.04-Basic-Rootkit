#!/bin/bash

RMMODULE=quick
INSMODULE=quick.ko

sudo rmmod $RMMODULE
make
sleep 1
sudo insmod $INSMODULE
sleep 1
tail /var/log/kern.log
