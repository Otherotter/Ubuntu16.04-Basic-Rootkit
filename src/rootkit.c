
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
//#include <string.h>
#include "rootkit_conf.conf.h"

MODULE_LICENSE("GPL") ;
MODULE_AUTHOR("Brendan<brendanfoley1214@gmail.com>") ;
MODULE_DESCRIPTION("Rootkit Testfile for CSE331") ;
MODULE_VERSION("0.3");

#define PROC_VERSION "/proc/version"
#define BOOT_PATH "/boot/System.map-"
/*
char *get_sys_call_table(char *path){
	struct file *f;
	mm_segment_t old_fs;
	char buff[256];
	char *kern_ver = kmalloc(256, GFP_KERNEL);

	memset(buff, 0, 256);

	f = filp_open(PROC_VERSION, O_RDONLY, 0);
	// Error handling failing... Program crashes on above line.
	/*if(f == NULL){
		printk(KERN_WARNING "Unable to open file: %s\n", PROC_VERSION);
		return;
	}*/

/*
	old_fs = get_fs();
	set_fs(KERNEL_DS);

	f->f_op->read(f, buff, 256, &f->f_pos);


	set_fs(old_fs);
	return 1;
}*/

char *get_kernel_version(void){
	struct file *f;
	mm_segment_t old_fs;
	char buff[256];
	char *kern_ver = kmalloc(256, GFP_KERNEL);

	int i;
	memset(buff, 0, 256);

	f = filp_open(PROC_VERSION, O_RDONLY, 0);
	// Error handling failing... Program crashes on above line.
	/*if(f == NULL){
		printk(KERN_WARNING "Unable to open file: %s\n", PROC_VERSION);
		return;
	}*/
	old_fs = get_fs();
	set_fs(KERNEL_DS);

	f->f_op->read(f, buff, 256, &f->f_pos);
	
	i = 14;
	while(i < 256 && buff[i] != ' '){
		kern_ver[i-14] = buff[i];
		i++;
	}
	kern_ver[i] = '\0';
	set_fs(old_fs);
	return kern_ver;
}


// Loads the LKM
static int __init rootkit_init(void){
	printk(KERN_INFO "Hello Kernel!");
	char *kernel_version = get_kernel_version();
	char *sys_call_address;
	if(kernel_version == NULL){
		return 1;
	}
	char path[40] = BOOT_PATH;
	strcat(path, kernel_version);
	printk(KERN_INFO "Full Boot path is: %s\n", path);
	//*sys_call_address = find_sys_call_table(*path);
	return 0;
}


// Exits the LKM
static void __exit rootkit_exit(void){
	printk(KERN_INFO "Rootkit Unloaded\n");
}


module_init(rootkit_init);
module_exit(rootkit_exit);
