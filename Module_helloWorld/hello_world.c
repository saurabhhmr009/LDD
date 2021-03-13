#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Saurabh Bhamra");
MODULE_DESCRIPTION("Hello World Module");
MODULE_VERSION("1.0");

static int hello_init(void) {
    printk(KERN_INFO "Hello World Module!!!\n");
    return 0;
}

static void hello_exit(void) {
    printk(KERN_INFO "Goodbye!!!\n");
}

module_init(hello_init);
module_exit(hello_exit);
