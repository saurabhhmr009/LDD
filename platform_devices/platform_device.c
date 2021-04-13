#include "platform.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Saurabh Bhamra");
MODULE_DESCRIPTION("Platform device code.");
MODULE_VERSION("1.0");

// Initialize the platform data of the devices
struct platform_dev_data pform_data[2] = {
    [0] = {
        .size = dev_size1,
        .permission = RDWR,
        .serial_number = "PDEVXYZ11001"
    },

    [1] = {
        .size = dev_size2,
        .permission = RDWR,
        .serial_number = "PDEVXYZ11002"
    }
};

// Initialize the platform device structure instances
struct platform_device platform_dev1 = {
    .name = "char-device",
    .id = 0,
    .dev = {
        .platform_data = &pform_data[0]
    }
};

struct platform_device platform_dev2 = {
    .name = "char-device",
    .id = 2,
    .dev = {
        .platform_data = &pform_data[1]
    }
};

// Init method for platform device
static int __init platform_dev_init(void) {
    platform_device_register(&platform_dev1);
    platform_device_register(&platform_dev2);
    return 0;
}

// Exit method for the plaform devices
static void __exit platform_dev_exit(void) {
    platform_device_unregister(&platform_dev1);
    platform_device_unregister(&platform_dev2);
}

// Initialize the  init and exit methods.
module_init(platform_dev_init);
module_exit(platform_dev_exit);