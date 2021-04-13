#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>

// Define the permissions and size for platform device
#define RDONLY 0x01
#define WRONLY 0x01
#define RDWR 0x11
#define dev_size1 512
#define dev_size2 1024

// Define macro for the printing of messages.
#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt,__func__

// Structure for the private data of the platform device
struct platform_dev_data {
    int size;
    int permission;
    const char *serial_number;
};