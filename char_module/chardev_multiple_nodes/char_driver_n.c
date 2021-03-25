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


static int __init chr_driver_init(void) {
	return 0;
}

static void __exit chr_driver_exit(void) {

}

module_init(chr_driver_init);
module_exit(chr_driver_exit);