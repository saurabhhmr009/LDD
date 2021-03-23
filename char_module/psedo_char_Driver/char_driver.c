#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>

#define DEV_MEM_SIZE 512
#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt,__func__

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Saurabh Bhamra");
MODULE_DESCRIPTION("A simple character driver");

char dev_buff[DEV_MEM_SIZE];
static dev_t dev_num;

/* Initialize the cdev structure. */
struct cdev char_cdev;
struct class *dev_class;
struct device *char_device;

static loff_t dev_lseek(struct file *filp, loff_t offset, int whence) {
	loff_t temp;
	pr_info("lseek requested.\n");
	pr_info("Current value of the file position %lld\n", filp->f_pos);

	switch(whence) {
		case SEEK_SET:{
			if((offset > DEV_MEM_SIZE) || (offset < 0)) {
				return -EINVAL;
			}
			filp->f_pos = offset;
			break;
		}
		case SEEK_CUR: {
			temp = filp->f_pos + offset;
			if((temp > DEV_MEM_SIZE) || (temp < 0)) {
				return -EINVAL;
			}
			filp->f_pos = temp;
			break;
		}
		case SEEK_END: {
			temp = DEV_MEM_SIZE + offset;
			if((temp > DEV_MEM_SIZE) || (temp < 0)) {
				return -EINVAL;
			}
			filp->f_pos = temp;
			break;
		}
		default: {
			return -EINVAL;
		}
	}
	pr_info("New value of the file position %lld\n", filp->f_pos);
	return filp->f_pos;
}

static ssize_t dev_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos) {
	pr_info("Device Read requested for %zu bytes\n", count);
	pr_info("Current file position: %lld\n", *f_pos);

	if(*f_pos + count > DEV_MEM_SIZE) {
		count = DEV_MEM_SIZE - *f_pos;
	}
	if(copy_to_user(buff, &dev_buff[*f_pos], count)) {
		return -EFAULT;
	}
	*f_pos += count;

	pr_info("Number of bytes Successfully read: %zu bytes\n", count);
	pr_info("Updated file position  = %lld\n", *f_pos);

	return count;
}

static ssize_t dev_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos) {
	pr_info("Device write requested for %zu bytes\n", count);
	pr_info("Current file position: %lld\n", *f_pos);

	if(*f_pos + count > DEV_MEM_SIZE) {
		count = DEV_MEM_SIZE - *f_pos;
	}
	if(!count){
		pr_err("No space left on the deivce driver memory\n.");
		return -ENOMEM;
	}
	if(copy_from_user(&dev_buff[*f_pos], buff, count)) {
		return -EFAULT;
	}
	*f_pos += count;

	pr_info("Number of bytes Successfully read: %zu bytes\n", count);
	pr_info("Updated file position  = %lld\n", *f_pos);

	return count;
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
		pr_info("Device number registration failed.\n");
		goto out;
	}
	pr_info("Device number Major:Minor = %d:%d\n", MAJOR(dev_num), MINOR(dev_num));

	cdev_init(&char_cdev, &char_fops);
	char_cdev.owner = THIS_MODULE;

	ret  = cdev_add(&char_cdev, dev_num, 1);
	if(ret < 0) {
		pr_info("Char structure registration failed\n");
		goto unreg_chrdev;
	}

	dev_class = class_create(THIS_MODULE, "char_class");
	if(IS_ERR(dev_class)) {
		pr_err("Class creation failed\n");
		ret = PTR_ERR(dev_class);
		goto cdev_del;
	}

	char_device = device_create(dev_class, NULL, dev_num, NULL, "chrnull");
	if(IS_ERR(char_device)) {
		pr_err("Device creation failed\n");
		ret = PTR_ERR(char_device);
		goto class_del;
	}

	pr_info("Device registration is done successful!!!\n");
	return 0;

class_del:
	class_destroy(dev_class);
cdev_del:
	cdev_del(&char_cdev);
unreg_chrdev:
	unregister_chrdev_region(dev_num, 1);
out:
	return ret;
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