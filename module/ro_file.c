#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("homework3");
MODULE_AUTHOR("Yesheng Ma");

static int proc_show(struct seq_file *m, void *v) {
    seq_printf(m, "Message from a Linux kernel module.\n");
    return 0;
}

static int proc_open(struct inode *inode, struct file *file) {
    return single_open(file, proc_show, NULL);
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open  = proc_open,
    .read  = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static int __init homework3_init(void) {
    struct proc_dir_entry *entry;
    entry = proc_create("homework3", 0, NULL, &fops);
    if (!entry) {
        return -ENOMEM;
    } else {
        printk(KERN_INFO "Proc_read_entry created successfully.\n");
        return 0;
    }
}

static void __exit homework3_exit(void) {
    remove_proc_entry("homework3", NULL);
    printk(KERN_INFO "Goodbye!\n");
}

module_init(homework3_init);
module_exit(homework3_exit);

