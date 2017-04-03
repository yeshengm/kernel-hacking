#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include <linux/highmem.h>
#include <linux/init.h>
#include <linux/string.h>
#include <asm/uaccess.h>

static int __init mtest_init(void)
{
	int fuck = *(int *) 0xffff88001df3d000;
	printk("kernel memory to: 0x%x\n", fuck);
	return 0;
}

static void __exit mtest_exit(void)
{
}

module_init(mtest_init);
module_exit(mtest_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yesheng Ma");

