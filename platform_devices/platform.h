#include <linux/module.h>
#include <linux/platform_device.h>

// Define the permissions and size for platform device
#define RDONLY 0x01
#define WRONLY 0x01
#define RDWR 0x11
#define dev_size1 512
#define dev_size2 1024

// Structure for the private data of the platform device
struct platform_dev_data {
    int size;
    int permission;
    const char *serial_number;
};