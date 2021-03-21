#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/device.h>

// Global variable to define the first of dev_t type which holds major and minor number
static dev_t first;
// Global variable for the char device structure.
static struct cdev c_dev;
// Global varible for the device class.
static struct class *dev_class;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Saurabh Bhamra");
MODULE_DESCRIPTION("A Simple Char driver");
MODULE_VERSION("1.0");

// Mapping the file operations defined in the fops structure.
static int dev_open(struct inode *i, struct file *f) {
	printk(KERN_INFO "Opened!!\n");
	return 0;
}
static int dev_close(struct inode *i, struct file *f) {
	printk(KERN_INFO "Closed!!\n");
	return 0;
}

static ssize_t dev_read(struct file *f, char __user *buff, size_t count, loff_t *offp) {
	printk(KERN_INFO "Read!!\n");
	return 0;
}

static ssize_t dev_write(struct file *f, const char __user *buff, size_t count, loff_t *off) {
	printk(KERN_INFO "Write!!\n");
	return count;
}

static struct file_operations pugs_fops = 
{
	.owner = THIS_MODULE,
	.open = dev_open,
	.release = dev_close,
	.read = dev_read,
	.write = dev_write
};


static int init_char_dev(void) {
	int check;
	struct device *dev_ret;

	check = alloc_chrdev_region(&first, 0, 1, "charDevice");
	if(check < 0) {
		printk(KERN_INFO "Device registration failed!!\n");
		return check;
	}

	if(IS_ERR(dev_class = class_create(THIS_MODULE, "chardrv"))) {
		unregister_chrdev_region(first, 1);
		return PTR_ERR(dev_class);
	}

	if(IS_ERR(dev_ret = device_create(dev_class, NULL, first, NULL, "charNull"))) {
		class_destroy(dev_class);
		unregister_chrdev_region(first, 1);
		return PTR_ERR(dev_ret);
	}

	cdev_init(&c_dev, &pugs_fops);
	check = cdev_add(&c_dev, first, 1);
	if(check < 0) {
		device_destroy(dev_class, first);
		class_destroy(dev_class);
		unregister_chrdev_region(first, 1);
		return check;
	}
	printk(KERN_INFO "Device inserted!!\n");

	return 0;
}



static void exit_char_dev(void) {
	cdev_del(&c_dev);
	device_destroy(dev_class, first);
	class_destroy(dev_class);
	unregister_chrdev_region(first, 1);
	printk(KERN_INFO "Exiting the module!!\n");
}

module_init(init_char_dev);
module_exit(exit_char_dev);