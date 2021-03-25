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
	return 0;
}

static void __exit chr_driver_exit(void) {

}

module_init(chr_driver_init);
module_exit(chr_driver_exit);