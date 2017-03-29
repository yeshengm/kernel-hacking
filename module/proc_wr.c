#include <linux/module.h>  
#include <linux/sched.h>  
#include <linux/uaccess.h>  
#include <linux/proc_fs.h>  
#include <linux/fs.h>  
#include <linux/seq_file.h>  
#include <linux/slab.h>  
  
#define BUF_SIZE 10

static char *kern_buf = NULL;  
static int cur_len = 0;

static struct proc_dir_entry *file;
static struct proc_dir_entry *dir;
  
static int wr_proc_show(struct seq_file *m, void *v)  
{
    seq_printf(m, "%s", kern_buf);  
    return 0;  
}  
  
static ssize_t wr_proc_write(struct file *file, const char __user *buffer,
                             size_t count, loff_t *f_pos)  
{
    int copy_len= 0;
    char *dst = kern_buf + cur_len;
    if (count + cur_len > BUF_SIZE) {
        if (count > BUF_SIZE)
            copy_len = BUF_SIZE;
        else
            copy_len = count;
        dst = kern_buf;
        cur_len = copy_len;
    } else {
        copy_len = count;
        cur_len += count;
    }

    printk(KERN_INFO "copied char size is %d", copy_len);
    if(copy_from_user(dst, buffer, copy_len)) {
        return EFAULT;  
    }

    dst[cur_len] = '\0';
    return count; 
}  
  
static int wr_proc_open(struct inode *inode, struct file *file)  
{  
    return single_open(file, wr_proc_show, NULL);  
}  
  
static struct file_operations wr_fops = {  
    .owner   = THIS_MODULE,  
    .open    = wr_proc_open,  
    .release = single_release,  
    .read    = seq_read,  
    .llseek  = seq_lseek,  
    .write   = wr_proc_write,  
};  
  
static int __init wr_init(void)  
{  
    kern_buf = kzalloc((BUF_SIZE+1), GFP_KERNEL);  

    dir = proc_mkdir("hello", NULL);
    file = proc_create("world", 0666, dir, &wr_fops);  
    if(!file)  
        return -ENOMEM;  
    return 0;  
}  
  
static void __exit wr_exit(void)  
{  
    remove_proc_entry("world", dir);  
    remove_proc_entry("hello", NULL);
    kfree(kern_buf);
}  
  
module_init(wr_init);  
module_exit(wr_exit);  

MODULE_LICENSE("GPL");  
MODULE_AUTHOR("Yesheng Ma");  


