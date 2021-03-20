#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Saurabh Bhamra");
MODULE_DESCRIPTION("Major and Minor Number");
MODULE_VERSION("1.0");

static dev_t first;

static int init_charDriv(void) {
	int check;

	//Registering the driver namer "char_device"
	check = alloc_chrdev_region(&first, 0, 2, "char_device");
	if(check < 0) {
		printk(KERN_INFO "Unable to get register the driver.\n");
		return check;
	}
	else {
		printk(KERN_INFO "Major, Minor: %d %d \n", MAJOR(first), MINOR(first));
	}
	return 0;
}

void exit_charDriv(void) {
	unregister_chrdev_region(first, 3);
	printk(KERN_INFO "Exiting the Char Device driver.\n");
}

module_init(init_charDriv);
module_exit(exit_charDriv);