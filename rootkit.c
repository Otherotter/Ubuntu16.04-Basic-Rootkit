/* rootkit..c */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include "rootkit_conf.conf.h"

MODULE_LICENSE("GPL") ;
MODULE_AUTHOR("Brendan<brendanfoley1214@gmail.com>") ;
MODULE_DESCRIPTION("Rootkit for CSE331") ;
MODULE_VERSION("0.1");

static int __init rootkit_init(void){
	printk(KERN_INFO "Rootkit Loaded\n");
}

static int __exit rootkit_exit(void){
	printk(KERN_INFO "Rootkit Unloaded\n");
}

module_init(rootkit_init);
module_exit(rootkit_exit);