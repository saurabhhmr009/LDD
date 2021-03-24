#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>

#define NO_OF_DEVICES 4
#define DEV_MEM_SIZE1 1024
#define DEV_MEM_SIZE2 512
#define DEV_MEM_SIZE3 1024
#define DEV_MEM_SIZE4 512


#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt,__func__

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Saurabh Bhamra");
MODULE_DESCRIPTION("A simple character driver with n devices");

char dev_buff1[DEV_MEM_SIZE1];
char dev_buff2[DEV_MEM_SIZE2];
char dev_buff3[DEV_MEM_SIZE3];
char dev_buff4[DEV_MEM_SIZE4];

struct chrdev_private_data {
	char *buffer;
	unsigned int size;
	const char *serial_number;
	int permission;
	struct cdev char_cdev;
};

struct chrdrv_private_data {
	int total_devices;
	dev_t dev_num;
	struct class *dev_class;
	struct device *char_device;
	struct chrdev_private_data chrdev_data[NO_OF_DEVICES];
};


struct chrdrv_private_data chrdrv_data = {
	.total_devices = NO_OF_DEVICES,
	.chrdev_data = {
		[0] = {
			.buffer = dev_buff1,
			.size = DEV_MEM_SIZE1,
			.serial_number = "CHRDEV123X0",
			.permission = 0x1 /*RDONLY*/
		},
		[1] = {
			.buffer = dev_buff2,
			.size = DEV_MEM_SIZE2,
			.serial_number = "CHRDEV123X1",
			.permission = 0x10
		},
		[2] = {
			.buffer = dev_buff3,
			.size = DEV_MEM_SIZE3,
			.serial_number = "CHRDEV123X2",
			.permission = 0x11
		},
		[3] = {
			.buffer = dev_buff4,
			.size = DEV_MEM_SIZE4,
			.serial_number = "CHRDEV123X3",
			.permission = 0x11
		}
	}
};

static loff_t dev_lseek(struct file *filp, loff_t offset, int whence) {
	/*loff_t temp;
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
	return filp->f_pos;*/
	return 0;
}

static ssize_t dev_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos) {
	struct chrdev_private_data *chrdev_data = (struct chrdev_private_data *)filp->private_data;
	int max_size = chrdev_data->size;

	pr_info("Device Read requested for %zu bytes\n", count);
	pr_info("Current file position: %lld\n", *f_pos);

	if(*f_pos + count > max_size) {
		count = max_size - *f_pos;
	}
	if(copy_to_user(buff, chrdev_data->buffer+(*f_pos), count)) {
		return -EFAULT;
	}
	*f_pos += count;

	pr_info("Number of bytes Successfully read: %zu bytes\n", count);
	pr_info("Updated file position  = %lld\n", *f_pos);

	return count;
}

static ssize_t dev_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos) {
	struct chrdev_private_data *chrdev_data = (struct chrdev_private_data *)filp->private_data;
	int max_size = chrdev_data->size;

	pr_info("Device write requested for %zu bytes\n", count);
	pr_info("Current file position: %lld\n", *f_pos);

	if(*f_pos + count > max_size) {
		count = max_size - *f_pos;
	}
	if(!count){
		pr_err("No space left on the deivce driver memory\n.");
		return -ENOMEM;
	}
	if(copy_from_user(chrdev_data->buffer+(*f_pos), buff, count)) {
		return -EFAULT;
	}
	*f_pos += count;

	pr_info("Number of bytes Successfully read: %zu bytes\n", count);
	pr_info("Updated file position  = %lld\n", *f_pos);

	return count;
}

int check_permission(void) {
	return 0;
}

static int dev_open(struct inode *inode, struct file *filep) {
	int minor_num, ret;
	struct chrdev_private_data *chrdev_data;

	minor_num = MINOR(inode->i_rdev);
	pr_info("Minor number of the device is %d\n", minor_num);

	chrdev_data = container_of(inode->i_cdev, struct chrdev_private_data, char_cdev);
	filep->private_data = chrdev_data;

	ret = check_permission();
	(!ret)?pr_info("Open was successful\n"):pr_info("Open was unsuccessful\n");
	return ret;
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
	int ret, i;
	ret  = alloc_chrdev_region(&chrdrv_data.dev_num, 0, NO_OF_DEVICES, "chrDevice");
	if(ret < 0) {
		pr_info("Device number registration failed.\n");
		goto out;
	}

	chrdrv_data.dev_class = class_create(THIS_MODULE, "char_class");
	if(IS_ERR(chrdrv_data.dev_class)) {
		pr_err("Class creation failed\n");
		ret = PTR_ERR(chrdrv_data.dev_class);
		goto unreg_chrdev;
	}
	for(i = 0; i<NO_OF_DEVICES; i++) {
		pr_info("Device number Major:Minor = %d:%d\n", MAJOR(chrdrv_data.dev_num + i), MINOR(chrdrv_data.dev_num + i));

		cdev_init(&chrdrv_data.chrdev_data[i].char_cdev, &char_fops);
		chrdrv_data.chrdev_data[i].char_cdev.owner = THIS_MODULE;

		ret  = cdev_add(&chrdrv_data.chrdev_data[i].char_cdev, chrdrv_data.dev_num + i, 1);
		if(ret < 0) {
			pr_info("Char structure registration failed\n");
			goto del_cdev;
		}

		chrdrv_data.char_device = device_create(chrdrv_data.dev_class, NULL, chrdrv_data.dev_num + i, NULL, "chrdev%d", i);
		if(IS_ERR(chrdrv_data.char_device)) {
			pr_err("Device creation failed\n");
			ret = PTR_ERR(chrdrv_data.char_device);
			goto class_del;
		}
	}

	pr_info("Device registration is done successful!!!\n");
	return 0;

del_cdev:
class_del:
	for(; i>=0; i--) {
		device_destroy(chrdrv_data.dev_class, chrdrv_data.dev_num + i);
		cdev_del(&chrdrv_data.chrdev_data[i].char_cdev);
	}
	class_destroy(chrdrv_data.dev_class);

unreg_chrdev:
	unregister_chrdev_region(chrdrv_data.dev_num, NO_OF_DEVICES);
out:
	return ret;
}

static void exit_charDrive(void){
	int i;
	for(i = 0; i< NO_OF_DEVICES; i++) {
		device_destroy(chrdrv_data.dev_class, chrdrv_data.dev_num + i);
		cdev_del(&chrdrv_data.chrdev_data[i].char_cdev);
	}
	class_destroy(chrdrv_data.dev_class);
	unregister_chrdev_region(chrdrv_data.dev_num, NO_OF_DEVICES);
	pr_info("Module unloaded successful.\n");
}

module_init(init_charDrive);
module_exit(exit_charDrive);