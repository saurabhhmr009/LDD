/* Code for the platform driver side.
 */
#include "platform.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Saurabh Bhamra");
MODULE_DESCRIPTION("Platform driver code.");
MODULE_VERSION("1.1");

// Function gets called when device is inserted.
int char_platform_probe(struct platform_device *pdriver) {
    pr_info("A device is detected.\n");
    return 0;
}

// function gets called when device is removed.
int  char_platform_remove(struct platform_device *pdriver) {
    pr_info("A device is removed.\n");
    return 0;
}
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

// Initialize the platform driver structure.
struct platform_driver chardev_platform = {
    .probe = char_platform_probe,
    .remove = char_platform_remove,
    .driver = {
        .name = "char-device"
    }
};

// Init function
static int __init char_platform_init(void) {
    platform_driver_register(&chardev_platform);
    pr_info("Platform driver inserted.\n");
    return 0;
}

//Exit function
static void __exit char_platform_exit(void) {
    platform_driver_unregister(&chardev_platform);
    pr_info("Platform driver removed.\n");
}

module_init(char_platform_init);
module_exit(char_platform_exit);