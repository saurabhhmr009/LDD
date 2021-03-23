#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>

#define DEV_SIZE 512
#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt,__func__

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Saurabh Bhamra");
MODULE_DESCRIPTION("A simple character driver");

char dev_buff[DEV_SIZE];
static dev_t dev_num;

/* Initialize the cdev structure. */
struct cdev char_cdev;
struct class *dev_class;
struct device *char_device;

static loff_t dev_lseek(struct file *filp, loff_t off, int whence) {
	pr_info("lseek requested.\n");
	return 0;
}

static ssize_t dev_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos) {
	pr_info("Device Read requested for %zu bytes\n", count);
	return 0;
}

static ssize_t dev_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos) {
	pr_info("Device write requested for %zu bytes\n", count);
	return 0;
}

static int dev_open(struct inode *inode, struct file *filep) {
	pr_info("File open successful.\n");
	return 0;
}

static int dev_release(struct inode *inode, struct file*filp) {
	pr_info("File closed successful.\n");
	return 0;
}

/* Define the file operations to be done on the char device */
struct file_operations char_fops = {
	.owner  = THIS_MODULE,
	.open = dev_open,
	.write = dev_write,
	.read = dev_read,
	.release = dev_release,
	.llseek = dev_lseek
};

static int init_charDrive(void){
	int ret;
	ret  = alloc_chrdev_region(&dev_num , 0, 1, "chrDevice");
	if(ret < 0) {
		printk(KERN_INFO "Device number registration failed.\n");
		return ret;
	}
	pr_info("Device number Major:Minor = %d:%d\n", MAJOR(dev_num), MINOR(dev_num));

	cdev_init(&char_cdev, &char_fops);
	char_cdev.owner = THIS_MODULE;

	cdev_add(&char_cdev, dev_num, 1);

	dev_class = class_create(THIS_MODULE, "char_class");

	char_device = device_create(dev_class, NULL, dev_num, NULL, "chrnull");

	pr_info("Device registration is done successful!!!\n");
	return 0;
}

static void exit_charDrive(void){
	device_destroy(dev_class, dev_num);
	class_destroy(dev_class);
	cdev_del(&char_cdev);
	unregister_chrdev_region(dev_num, 1);
	pr_info("Module unloaded successful.\n");
}

module_init(init_charDrive);
module_exit(exit_charDrive);