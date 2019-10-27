# Rootkit
This is a Rootkit for CSE331

Using the uname command you are able to know system information. Running these command on the VM, these are the respective output:
    * uname -s (kernal-name): Linux
    * uname -v (kernal-version): #46~16.04.1-Ubuntu SMP Mon Dec 4 15:57:59 UTC 2017
    * uname -o (operating system): GNU/Linux

Linux File Structure: 
(source of file structure): https://www.howtogeek.com/117435/htg-explains-the-linux-directory-structure-explained/)

    * /bin – Essential User Binaries
        +   The /bin directory contains the essential user binaries (programs) that must be present when the system is mounted in
            single-user mode. Applications such as Firefox are stored in /usr/bin, while important system programs and utilities such as the bash shell are located in /bin. The /usr directory may be stored on another partition – placing these files in the /bin directory ensures the system will have these important utilities even if no other file systems are mounted. The /sbin directory is similar – it contains essential system administration binaries.

     * ./sbin – System Administration Binaries. 
        +   The /sbin directory is similar to the /bin directory. It contains essential binaries
            that are generally intended to be run by the root user for system administration.

    * ./proc – Kernel & Process Files
        +   The /proc directory similar to the /dev directory because it doesn’t contain
            standard files. It contains special files that represent system and process information.

    * ./usr – User Binaries & Read-Only Data
        +   The /usr directory contains applications and files used by users, as opposed to
            applications and files used by the system. For example, non-essential applications are located inside the /usr/bin directory instead of the /bin directory and non-essential system administration binaries are located in the /usr/sbin directory instead of the /sbin directory. Libraries for each are located inside the /usr/lib directory. The /usr directory also contains other directories – for example, architecture-independent files like graphics are located in /usr/share. The /usr local directory is where locally compiled applications install to by default – this prevents them from mucking up the rest of the system.

    * /lib – Essential Shared Libraries
        +   The /lib directory contains libraries needed by the essential binaries in the /bin and /sbin folder. Libraries needed by     the binaries in the /usr/bin folder are located in /usr/lib.


Kernal module:
(source): {https://unix.stackexchange.com/questions/47330/what-exactly-are-linux-kernel-headers, 
https://unix.stackexchange.com/questions/27042/what-does-a-kernel-source-tree-contain-is-this-related-to-linux-kernel-headers,
https://www.kernel.org/doc/html/latest/kbuild/modules.html,
https://en.wikipedia.org/wiki/Loadable_kernel_module,
https://linux-kernel-labs.github.io/master/labs/kernel_modules.html,
http://tldp.org/LDP/lkmpg/2.6/html/lkmpg.html#AEN181
}

    * Loadable kernel module (LMK) is an object file that contains code to extend the running kernel, or so-called base kernel, of an operating system. LKMs are typically used to add support for new hardware (as device drivers) and/or filesystems, or for adding system calls. When the functionality provided by a LKM is no longer required, it can be unloaded in order to free memory and other resources.
        +   The adventage of having LMK is that they stop the operating system from having to include all possible anticipated
            functionality already compiled directly into the base kernel. Much of this  would reside in memory without being used, wasting memory, and would require that users rebuild and reboot the base kernel every time they require new functionality.
        +   Disadventage of having LMK is that there could be a fragmetation penalty. On the contrary, a base kernal is unpacked
            into real contiguous memory by its setup routines; thus, the base kernel code is never fragmented. So, because an LMK is "outside code", there's a possible of the code crashing.
        +   In terms of security,  LMK are a convenient method of modifying the running kernel, this can be abused by attackers on a
            compromised system to prevent detection of their processes or files, allowing them to maintain control over the system. Many rootkits make use of LKMs in this way. Note that on most operating systems modules do not help privilege elevation in any way, as elevated privilege is required to load a LKM; they merely make it easier for the attacker to hide the break-in.
                -   Linux allows disabling module loading via sysctl option /proc/sys/kernel/modules_disabled. An initramfs (needs 
                    more outside information) system may load specific modules needed for a machine at boot and then disable module loading. This makes the security very similar to a monolithic kernel. If an attacker can change the initramfs, they can change the kernel binary.


Steps for the hello.c  kernel module.
Follow this source: http://tldp.org/LDP/lkmpg/2.6/html/lkmpg.html#AEN181
	* Created hello-1.c file
	* In the terminal type: gedit Makefile
	* A text editor should have popped up for Makefile
	* Include everything from chapter 2.2 of the source in this Makefile file
	* Now you are going to covert it into a readable kernal mod. Type in: make
	* Now you must install the mod. In the terminal type: sudo insmod ./hello-1.ko	
	* The mod should be installed. Type in: cat /proc/modules 
	* To verify that the mod is functioning correct. Type in: cat /var/log/kern.log
	* remove mod. Type in cat /var/log/kern.log  





bib:
    * https://blog.sourcerer.io/writing-a-simple-linux-kernel-module-d9dc3762c234
        + Discuss Linux Modules.

