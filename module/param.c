#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel module params");
MODULE_AUTHOR("Yesheng Ma");

static int int_arg = 0;
static char *charp_arg = "world";
static int int_arr_arg[2] = {-1, 1};
static int int_arr_argc = 0;

module_param(int_arg, int, 0644);
module_param(charp_arg, charp, 0644);
module_param_array(int_arr_arg, int, &int_arr_argc, 0644);

static int __init param_init(void) {
    int i;
    printk(KERN_INFO "Params: int arg: %d\n", int_arg);
    printk(KERN_INFO "Params: charp arg: %s\n", charp_arg);
    for (i = 0; i < (sizeof int_arr_arg / sizeof (int)); i++) {
        printk(KERN_INFO "Params: int_arr_arg[%d] = %d", i, int_arr_arg[i]);
    }
    printk(KERN_INFO "  actually get %d int arr args", int_arr_argc);
    return 0;
}

static void __exit param_exit(void) {
    printk(KERN_INFO "Goodbye world!\n");
}

module_init(param_init);
module_exit(param_exit);
