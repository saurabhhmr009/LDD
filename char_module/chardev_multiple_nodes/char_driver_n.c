/* A program which implements a Character driver with n devices in a Linux system.
 * At the moment the current program implements 4 devices only.
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>

// Define macro for the memory size for the each device.
#define DEV1_MEM_SIZE 1024
#define DEV2_MEM_SIZE 512
#define DEV3_MEM_SIZE 1024
#define DEV4_MEM_SIZE 512

// Define macros for the permissions.
#define RDONLY 0x01
#define WRONLY 0x01
#define RDWR 0x11

//Define macro for total number of devices
#define NO_OF_DEVICES 4

// Define macro for the printing of messages.
#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt,__func__

//Define char arrays which will be used in the Kernel Space.
char dev_buff1[DEV1_MEM_SIZE];
char dev_buff2[DEV2_MEM_SIZE];
char dev_buff3[DEV3_MEM_SIZE];
char dev_buff4[DEV4_MEM_SIZE];

//Define a structure which holds the private data of the device.
struct chrdev_private_data {
	char *buffer;  					// Holds the starting address of the memory of the device.
	unsigned int size; 				// Holds the MAX memory size of the device.
	const char *serial_number;		// Holds the serial number of the device.
	int permission;					// Holds the permission in which the device could be accessed(RDWR,RDONLY,WRONLY etc.)
	struct cdev char_dev;			// Every device will have its own cdev structure registered with VHS.
};

//Define a structure which holds the private data for the driver.
struct chrdrv_private_data {
	int total_devices;				// Holds the number of devices for the driver.
	dev_t device_number;			// Holds the device number of the driver.
	struct class *char_class;
	struct device *char_device;
	struct chrdev_private_data chrdev_data[NO_OF_DEVICES];
};

// Initialize the chrdrv_private_data structure.
struct chrdrv_private_data chrdrv_data = {
	.total_devices = NO_OF_DEVICES,
	.chrdev_data = {
		[0] = {
			.buffer = dev_buff1,
			.size = DEV1_MEM_SIZE,
			.serial_number = "CHRDEVXYZ00",
			.permission  = RDONLY
		},
		[1] = {
			.buffer = dev_buff2,
			.size = DEV2_MEM_SIZE,
			.serial_number = "CHRDEVXYZ01",
			.permission = WRONLY
		},
		[2] = {
			.buffer = dev_buff3,
			.size = DEV3_MEM_SIZE,
			.serial_number = "CHRDEVXYZ02",
			.permission = RDWR
		},
		[3] = {
			.buffer = dev_buff4,
			.size = DEV4_MEM_SIZE,
			.serial_number = "CHRDEVXYZ04",
			.permission = WRONLY
		}
	}
};

// Device function to change the seek position of f_pos
static loff_t dev_lseek(struct file *filp, loff_t offset, int whence) {
	return 0;
}

// Device function to read the data from the device to user space.
static ssize_t dev_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos) {
	return 0;
}

// Device function to write the data from user space to device.
static ssize_t dev_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos) {
	return 0;
}

// Device function to open the device file.
static int dev_open (struct inode *inode, struct file *filp) {
	return 0;
}

// Device function to close the device file.
static int dev_close(struct inode *inode, struct file *filp){
	return 0;
}

// Define a structure which maps the system calls with the device define calls.
struct file_operations char_ops = {
	.owner = THIS_MODULE,
	.open = dev_open,
	.release  = dev_close,
	.read = dev_read,
	.write = dev_write,
	.llseek = dev_lseek
};


static int __init chr_driver_init(void) {
	int ret, i;

	// Get the Device number for the driver
	ret = alloc_chrdev_region(&chrdrv_data.device_number, 0, NO_OF_DEVICES, "chardriver");
	if(ret < 0) {
		pr_info("Unable to get the device number for the driver.\n");
		goto out;
	}

	// Create the device class in the /sys/class directory.
	chrdrv_data.char_class = class_create(THIS_MODULE, "chrdev");
	if(IS_ERR(chrdrv_data.char_class)) {
		pr_err("Failed to register the class for the driver\n");
		ret = PTR_ERR(chrdrv_data.char_class);
		goto unregister_chrdev;
	}

	for(i = 0; i<NO_OF_DEVICES; i++) {
		pr_info("Major and Minor number are %d and %d\n", MAJOR(chrdrv_data.device_number + i), MINOR(chrdrv_data.device_number + i));

		// Initialize and add the char structure with the VHS.
		cdev_init(&chrdrv_data.chrdev_data[i].char_dev, &char_ops);
		ret = cdev_add(&chrdrv_data.chrdev_data[i].char_dev, chrdrv_data.device_number + i, 1);
		if(ret < 0) {
			pr_err("Character device structure initialization failed.\n");
			goto char_del;
		}

		chrdrv_data.char_device = device_create(chrdrv_data.char_class, NULL, chrdrv_data.device_number, NULL, "chrdrv%d", i);
		if(IS_ERR(chrdrv_data.char_device)) {
			pr_err("Device creation failed.\n");
			goto dev_del;
		}
	}
	return 0;

// Definations of the all goto functions(Might be wrong).
dev_del:
	for(; i>=0; i--) {
		device_destroy(chrdrv_data.char_class, chrdrv_data.device_number + i);
		cdev_del(&chrdrv_data.chrdev_data[i].char_dev);
	}
	class_destroy(chrdrv_data.char_class);

char_del:
	for(; i>=0; i--) {
		cdev_del(&chrdrv_data.chrdev_data[i].char_dev);
	}
	class_destroy(chrdrv_data.char_class);

unregister_chrdev:
	unregister_chrdev_region(chrdrv_data.device_number, NO_OF_DEVICES);

out:
	return ret;

}

static void __exit chr_driver_exit(void) {

}

module_init(chr_driver_init);
module_exit(chr_driver_exit);