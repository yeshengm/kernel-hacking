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

// print vma of current process
static void list_vma(void)
{
	struct mm_struct *mm = current->mm;
	struct vm_area_struct *vma;
	char a[4] = {'-', '-', '-'};
	int counter = 0;
	// hold the lock
	down_read(&mm->mmap_sem);
	printk("MTEST list vma:\n");
	for (vma = mm->mmap; vma; vma = vma->vm_next) {
		if (vma->vm_flags & VM_READ)
			a[0] = 'r';
		else
			a[0] = '-';
		if (vma->vm_flags & VM_WRITE)
			a[1] = 'w';
		else
			a[1] = '-';
		if (vma->vm_flags & VM_EXEC)
			a[2] = 'x';
		else
			a[2] = '-';
		counter++;
		printk("%d 0x%lx 0x%lx %c%c%c\n", counter, vma->vm_start, vma->vm_end, a[0], a[1], a[2]);
	}
	up_read(&mm->mmap_sem);
}
static struct page *seek_page(struct vm_area_struct *vma, unsigned long addr)
{
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	spinlock_t *ptl;

	struct page *page = NULL;
	struct mm_struct *mm = vma->vm_mm;

	pgd = pgd_offset(mm, addr);
	if (pgd_none(*pgd) || unlikely(pgd_bad(*pgd))) return NULL;

	pud = pud_offset(pgd, addr);
	if (pud_none(*pud) || unlikely(pud_bad(*pud))) return NULL;
	
	pmd = pmd_offset(pud, addr);
	if (pmd_none(*pmd) || unlikely(pmd_bad(*pmd))) return NULL;

	pte = pte_offset_map_lock(mm, pmd, addr, &ptl);
	if (!pte) return NULL;

	if (!pte_present(*pte)) {
		pte_unmap_unlock(pte, ptl);
		return NULL;
	}

	page = pfn_to_page(pte_pfn(*pte));
	if (!page) {
		pte_unmap_unlock(pte, ptl);
		return NULL;
	}

	get_page(page);
	pte_unmap_unlock(pte, ptl);
	return page;
}

static void find_page(unsigned long addr)
{
	struct vm_area_struct *vma;
	struct mm_struct *mm = current->mm;
	struct page *page;
	unsigned long kernel_addr;

	down_read(&mm->mmap_sem);
	printk("MTEST find page:\n");
	vma = find_vma(mm, addr);
	page = seek_page(vma, addr);

	if (!page) {
		printk("Translation not found\n");
		up_read(&mm->mmap_sem);
		return;
	}
	kernel_addr = (unsigned long) page_address(page);
	kernel_addr += (addr & ~PAGE_MASK);
	printk("Translate 0x%lx to kernel address 0x%lx\n", addr, kernel_addr);
	up_read(&mm->mmap_sem);
}

static void write_val(unsigned long addr, unsigned long val)
{
	struct vm_area_struct *vma;
	struct mm_struct *mm = current->mm;
	struct page *page;
	unsigned long kernel_addr;

	down_read(&mm->mmap_sem);
	printk("MTEST write val\n");
	vma = find_vma(mm, addr);
	if (vma && addr >= vma->vm_start && (addr + sizeof(val)) < vma->vm_end) {
		if (!(vma->vm_flags & VM_WRITE)) {
			printk("vma not writable at 0x%lx\n", addr);
			up_read(&mm->mmap_sem);
			return;
		}
		page = seek_page(vma, addr);
		if (!page) {
			printk("page 0x%lx not found\n", addr);
			up_read(&mm->mmap_sem);
			return;
		}

		kernel_addr = (unsigned long) page_address(page);
		kernel_addr += (addr & ~PAGE_MASK);
		*(unsigned long *) kernel_addr = val;
		printk("0x%lx written to address 0x%lx\n", val, kernel_addr);
		put_page(page);
	}
	up_read(&mm->mmap_sem);
}


static ssize_t mtest_write(struct file *file, const char __user *buffer, size_t count, loff_t *f_ops)
{
	char cmd[128];
	unsigned long v1, v2;

	if (count > sizeof(cmd))
		return -EINVAL;
	if (copy_from_user(cmd, buffer, count))
		return -EINVAL;
	// add a terminator at the end
	cmd[count] = '\0';
	if (memcmp(cmd, "listvma", 7) == 0) {
		list_vma();
	} else if (memcmp(cmd, "findpage", 8) == 0) {
		if (sscanf(cmd+8, "%lx", &v1) == 1)
			find_page(v1);
	} else if (memcmp(cmd, "writeval", 8) == 0) {
		if (sscanf(cmd+8, "%lx %lx", &v1, &v2) == 2)
			write_val(v1, v2);
	} else {
		printk(KERN_INFO "Invalid input\n");
	}
	return count;
}

static int mtest_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s", "You are reading from mtest\n");
	seq_printf(m, "%s", "for detailed info, please write to this file\n");
	return 0;
}

static int mtest_open(struct inode *inode, struct file *file)
{
	return single_open(file, mtest_show, NULL);
}

static struct file_operations mtest_fops =
{
	.owner		= THIS_MODULE,
	.open		= mtest_open,
	.read		= seq_read,
	.write		= mtest_write,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init mtest_init(void)
{
	struct proc_dir_entry *entry;

	printk(KERN_INFO "mtest installed\n");
	entry = proc_create("mtest", 0666, NULL, &mtest_fops);
	if (!entry) {
		return -1;
	} else {
		printk(KERN_INFO "wr_entry for mtest created\n");
		return 0;
	}
}

static void __exit mtest_exit(void)
{
	remove_proc_entry("mtest", NULL);
	printk(KERN_INFO "mtest removed\n");
}

module_init(mtest_init);
module_exit(mtest_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yesheng Ma");

