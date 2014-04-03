#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>


void asm_rdmsr32 (unsigned long num, unsigned long *a, unsigned long *d)
{
	asm volatile ("rdmsr"
		      : "=a" (*a), "=d" (*d)
		      : "c" (num));
}

static int my_open( struct inode *inode, struct file *filp ) 
{
	printk( "SSLAB : open" );
	*(int*)0 = 0x01;	
	return 0;
}

static struct file_operations vd_fops = {
	.open = my_open,
};


static __init init_hello(void){
	unsigned long eax, edx;
	printk(KERN_ALERT "Hack inserted!\n");
	return 0;
}

static __exit exit_hello(void){
        printk(KERN_ALERT "Hack removed!\n");
	return 0;
}


module_init(init_hello);
module_exit(exit_hello);
MODULE_LICENSE("GPL");
