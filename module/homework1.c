#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init homework1_init(void) {
    printk(KERN_INFO "Hello world!\n");
    return 0;
}

static void __exit homework1_exit(void) {
    printk(KERN_INFO "Goodbye world!\n");
}

module_init(homework1_init);
module_exit(homework1_exit);
