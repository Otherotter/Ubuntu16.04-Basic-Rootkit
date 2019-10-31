/* rootkit..c */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <stdlib.h>
#include "rootkit_conf.conf.h"

MODULE_LICENSE("GPL") ;
MODULE_AUTHOR("Brendan<brendanfoley1214@gmail.com>") ;
MODULE_DESCRIPTION("Rootkit for CSE331") ;
MODULE_VERSION("0.1");

#define PROC_VERSION "/path/version"
#define PATH "/boot/System.map-"

/*
 * Elevates the process euid/uid to root
 * **Will need to give a pointer to the process as an argument (not sure how to do that yet though)
 */
static void elevate_process(void){

}


/*
 * Lowers the process euid/uid to its original
 * **Will need to give a pointer to the process as an argument (not sure how to do that yet though)
 */
static void lower_proccess(void){

}

/*
 * Gets the kernel version to be appeneded onto the PATH for accessing SysCallTable
 */
static char *get_kernel_version(void){
	char *buff = malloc(sizeof(char) * 20);
	
}

// Loads the LKM
static int __init rootkit_init(void){
	printk(KERN_INFO "Rootkit Loaded\n");
}


// Exits the LKM
static int __exit rootkit_exit(void){
	printk(KERN_INFO "Rootkit Unloaded\n");
}

module_init(rootkit_init);
module_exit(rootkit_exit);