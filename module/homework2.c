#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Homework2");
MODULE_AUTHOR("Yesheng Ma");

static int test;
module_param(test, int, 0644);

static int __init homework2_init(void) {
    printk(KERN_INFO "Hello world!\n");
    printk(KERN_INFO "Params: test: %d\n", test);
    return 0;
}

static void __exit homework2_exit(void) {
    printk(KERN_INFO "Goodbye world!\n");
}

module_init(homework2_init);
module_exit(homework2_exit);
