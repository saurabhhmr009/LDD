#include <linux/module.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Saurabh Bhamra");
MODULE_DESCRIPTION("Platform device code.");
MODULE_VERSION("1.0");

// Initialize the platform device structure instances
struct platform_device platform_dev1 = {
    .name = "char-device",
    .id = 0
};

struct platform_device platform_dev2 = {
    .name = "char-device",
    .id = 2
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