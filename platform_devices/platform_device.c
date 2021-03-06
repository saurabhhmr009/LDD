#include "platform.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Saurabh Bhamra");
MODULE_DESCRIPTION("Platform device code.");
MODULE_VERSION("1.0");


// Function to release the dynamically allocated memory in platform devices
void platform_release(struct device *dev) {
    pr_info("Device released.\n");
}

// Initialize the platform data of the devices
struct platform_dev_data pform_data[4] = {
    [0] = {
        .size = dev_size1,
        .permission = RDWR,
        .serial_number = "PDEVXYZ11001"
    },

    [1] = {
        .size = dev_size2,
        .permission = RDWR,
        .serial_number = "PDEVXYZ11002"
    },

    [2] = {
        .size = dev_size1,
        .permission = RDONLY,
        .serial_number = "PDEVXYZ11003"
    },

    [3] = {
        .size = dev_size2,
        .permission = WRONLY,
        .serial_number = "PDEVXYZ11004"
    }
};

// Initialize the platform device structure instances
struct platform_device platform_dev1 = {
    .name = "pcdev-A1x",
    .id = 0,
    .dev = {
        .platform_data = &pform_data[0],
        .release = platform_release
    }
};

struct platform_device platform_dev2 = {
    .name = "pcdev-A2x",
    .id = 1,
    .dev = {
        .platform_data = &pform_data[1],
        .release = platform_release
    }
};

struct platform_device platform_dev3 = {
    .name = "pcdev-A3x",
    .id = 2,
    .dev = {
        .platform_data = &pform_data[2],
        .release = platform_release
    }
};

struct platform_device platform_dev4 = {
    //.name = "char-device",
    .name = "pcdev-A4x",
    .id = 3,
    .dev = {
        .platform_data = &pform_data[3],
        .release = platform_release
    }
};

struct platform_device *platform_pcdevs[] = {
    &platform_dev1,
    &platform_dev2,
    &platform_dev3,
    &platform_dev4
};

// Init method for platform device
static int __init platform_dev_init(void) {
    //platform_device_register(&platform_dev1);
    //platform_device_register(&platform_dev2);
    platform_add_devices(platform_pcdevs, ARRAY_SIZE(platform_pcdevs));
    pr_info("Platform device inserted.\n");
    return 0;
}

// Exit method for the plaform devices
static void __exit platform_dev_exit(void) {
    platform_device_unregister(&platform_dev1);
    platform_device_unregister(&platform_dev2);
    platform_device_unregister(&platform_dev3);
    platform_device_unregister(&platform_dev4);
    pr_info("Platform device removed.\n");
}

// Initialize the  init and exit methods.
module_init(platform_dev_init);
module_exit(platform_dev_exit);