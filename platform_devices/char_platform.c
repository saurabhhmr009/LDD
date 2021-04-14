/* Code for the platform driver side.
 */
#include "platform.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Saurabh Bhamra");
MODULE_DESCRIPTION("Platform driver code.");
MODULE_VERSION("1.1");

// Create the private data structure for the device
struct chardev_pvtdata {
    struct platform_dev_data platform_pvtdata;
    char *buffer;
    dev_t dev_num;
    struct cdev char_cdev;
};

// Create the private data structure for the driver
struct chardrv_pvtdata {
    int total_devices;
    dev_t dev_base;
    struct class *chardrv_class;
    struct device *chardrv_dev;
};

struct chardrv_pvtdata pdrv_data;

// Check permission for the driver
int check_permission(int dev_perm, int acc_mode) {
    if(dev_perm == RDWR) {
        return 0;
    }

    if((dev_perm == RDONLY) && ((acc_mode & FMODE_READ) && !(acc_mode & FMODE_WRITE))) {
        return 0;
    }
    
    if((dev_perm == WRONLY) && ((acc_mode & FMODE_WRITE) && !(acc_mode & FMODE_READ))) {
        return 0;
    }

    return -EPERM;
}

loff_t char_platform_lseek(struct file *filp, loff_t offset, int whence) {
    return 0;
}

ssize_t char_platform_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos){
    return 0;
}

ssize_t char_platform_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos) {
    return -ENOMEM;
}

int char_platform_open(struct inode *inode, struct file *filp) {
    pr_info("Driver opened successfully.\n");
    return 0;
}

int char_platform_close(struct inode *inode, struct file *filp){
    pr_info("Driver closed successfully.\n");
    return 0;
}

//Platform driver operations
struct file_operations char_platform_ops = {
    .owner = THIS_MODULE,
    .read = char_platform_read,
    .write = char_platform_write,
    .open = char_platform_open,
    .release = char_platform_close,
    .llseek = char_platform_lseek
};

// Function gets called when device is inserted.
int char_platform_probe(struct platform_device *pdevice) {
    int ret;
    struct chardev_pvtdata *dev_data;
    struct platform_dev_data *platform_pvtdata;

    pr_info("A device is detected.\n");

    // Check for the platform data provided by the device.
    platform_pvtdata = (struct platform_dev_data*)dev_get_platdata(&pdevice->dev);
    if(platform_pvtdata == NULL) {
        pr_info("No platform data available.\n");
        return -EINVAL;
    }

    // Assign the memory dynamically to store the platform data
    //dev_data = kzalloc(sizeof(*dev_data), GFP_KERNEL);
    dev_data = devm_kzalloc(&pdevice->dev, sizeof(*dev_data), GFP_KERNEL);
    if(!dev_data) {
        pr_info("Failed to allocate the memory.\n");\
        return -ENOMEM;
    }

    // Copy data from platform device to dev_data
    dev_data->platform_pvtdata.size = platform_pvtdata->size;
    dev_data->platform_pvtdata.permission = platform_pvtdata->permission;
    dev_data->platform_pvtdata.serial_number = platform_pvtdata->serial_number;

    pr_info("Device serial number: %s\n", dev_data->platform_pvtdata.serial_number );
    pr_info("Device size: %d\n", dev_data->platform_pvtdata.size);
    pr_info("Device permission: %d\n", dev_data->platform_pvtdata.permission);

    // Dynamically allocate the memory of the buffer in Device pvt data structure
    //dev_data->buffer = kzalloc(sizeof(dev_data->platform_pvtdata.size), GFP_KERNEL);
    dev_data->buffer = devm_kzalloc(&pdevice->dev, sizeof(dev_data->platform_pvtdata.size), GFP_KERNEL);
    if(!dev_data->buffer) {
        pr_info("Failed to allocate for the buffer.\n");
        devm_kfree(&pdevice->dev, dev_data);
        return -ENOMEM;
    }

    // Get the device number for the probe function.
    dev_data->dev_num = pdrv_data.dev_base + pdevice->id;

    // cdev functions
    cdev_init(&dev_data->char_cdev, &char_platform_ops);
    dev_data->char_cdev.owner = THIS_MODULE;

    ret = cdev_add(&dev_data->char_cdev, dev_data->dev_num, 1);
    if(ret < 0) {
        pr_info("cdev for the device failed.\n");
        devm_kfree(&pdevice->dev, dev_data->buffer);
        devm_kfree(&pdevice->dev, dev_data);
        return ret;
    }

    pdrv_data.chardrv_dev = device_create(pdrv_data.chardrv_class, NULL, dev_data->dev_num, NULL, "chrdrv%d", pdevice->id);
    if(IS_ERR(pdrv_data.chardrv_dev)) {
        pr_err("Device creation failed.\n");
        ret = PTR_ERR(pdrv_data.chardrv_dev);
        return ret;
    }

    dev_set_drvdata(&pdevice->dev, dev_data);
    pdrv_data.total_devices++;
    pr_info("Probe was successful.\n");
    return 0;
}

// Function gets called when device is removed. Cleanups the memory held by platform devices.
int  char_platform_remove(struct platform_device *pdevice) {
    struct chardev_pvtdata *dev_data = dev_get_drvdata(&pdevice->dev);

    device_destroy(pdrv_data.chardrv_class, dev_data->dev_num);
    cdev_del(&dev_data->char_cdev);
    //kfree(dev_data->buffer);
    //kfree(dev_data);

    pdrv_data.total_devices--;
    pr_info("A device is removed.\n");
    return 0;
}

// Initialize the structure array for the device id matching.
struct platform_device_id pdev_array[] = {
    [0] = {
        .name = "pcdev-A1x"
    },

    [1] = {
        .name = "pcdev-A2x"
    },

    [2] = {
        .name = "pcdev-A3x"
    },

    [3] = {
        .name = "pcdev-A4x"
    },

    { }
};

// Initialize the platform driver structure.
struct platform_driver chardev_platform = {
    .probe = char_platform_probe,
    .remove = char_platform_remove,
    .id_table = pdev_array,
    .driver = {
        .name = "char-device"
    }
};

// Init function
static int __init char_platform_init(void) {
    int ret;

    ret = alloc_chrdev_region(&pdrv_data.dev_base, 0, NO_OF_DEVICES, "chardriver");
    if(ret<0){
        pr_info("Unable to get the device number for the driver.\n");
        return ret;
    }

    pdrv_data.chardrv_class = class_create(THIS_MODULE, "chrdev");
    if(IS_ERR(pdrv_data.chardrv_class)) {
        pr_err("Failed to register the class for the driver\n");
        ret = PTR_ERR(pdrv_data.chardrv_class);
        unregister_chrdev_region(pdrv_data.dev_base, NO_OF_DEVICES);
        return ret;
    }

    ret = platform_driver_register(&chardev_platform);
    if(ret<0) {
        pr_info("Failed to register the platform driver/device.\n");
        class_destroy(pdrv_data.chardrv_class);
        unregister_chrdev_region(pdrv_data.dev_base, NO_OF_DEVICES);
    }

    pr_info("Platform driver inserted.\n");
    return 0;
}

//Exit function
static void __exit char_platform_exit(void) {
    platform_driver_unregister(&chardev_platform);
    class_destroy(pdrv_data.chardrv_class);
    unregister_chrdev_region(pdrv_data.dev_base, NO_OF_DEVICES);

    pr_info("Platform driver removed.\n");
}

module_init(char_platform_init);
module_exit(char_platform_exit);