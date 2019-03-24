#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/unistd.h>

#include <linux/cred.h>
#include <linux/string.h>

#define	NAME_LEN	64
#define	FILE_COUNT	10

MODULE_LICENSE("GPL");

uid_t userid = -1;
char filenames[FILE_COUNT][NAME_LEN];
int count = 0;

void **sctable;
asmlinkage int (*orig_sys_open)(const char __user *filename, int flags, umode_t mode);

asmlinkage int tracker_sys_open(const char __user *filename, int flags, umode_t mode) {	
	char fname[NAME_LEN];
	uid_t uid;
	int i = 0;
	
	uid = get_current_user()->uid.val;
	if (uid != userid)
		return orig_sys_open(filename, flags, mode);

	copy_from_user(fname, filename, NAME_LEN);
	
	if (count == FILE_COUNT) {
		for (i = 0; i < FILE_COUNT - 1; i++)
			strncpy(filenames[i], filenames[i+1], NAME_LEN - 1);
		count--;
	}

	strcpy(filenames[count], fname);
	count++;
	return orig_sys_open(filename, flags, mode);
}

static int tracker_proc_open(struct inode *inode, struct file *file) {
	return 0;
}

static int tracker_proc_release(struct inode *inode, struct file *file) {
	return 0;
}

static ssize_t tracker_proc_read(struct file *file, char __user *ubuf, size_t size, loff_t *offset) {
	char buf[FILE_COUNT * (NAME_LEN + 1)] = { 0x0, };
	ssize_t toread;
	int i = 0;

	for (i = 0; i < count; i++) {
		strncat(buf, filenames[i], NAME_LEN - 1);
		strcat(buf, "\n");
	}
	
	toread = strlen(buf) >= *offset + size ? size : strlen(buf) - *offset;
	if (copy_to_user(ubuf, buf + *offset, toread))
		return -EFAULT;

	*offset = *offset + toread;
	return toread;
}

static ssize_t tracker_proc_write(struct file *file, const char __user *ubuf, size_t size, loff_t *offset) {
	char buf[128];

	if (*offset != 0 || size > 128)
		return -EFAULT;

	if (copy_from_user(buf, ubuf, size))
		return -EFAULT;

	userid = simple_strtol(buf, NULL, 10);
	*offset = strlen(buf);
	return *offset;
}

static const struct file_operations tracker_fops = {
	.owner =	THIS_MODULE,
	.open =		tracker_proc_open,
	.read =		tracker_proc_read,
	.write =	tracker_proc_write,
	.llseek =	seq_lseek,
	.release =	tracker_proc_release,
};

static int __init tracker_init(void) {
	unsigned int level;
	pte_t *pte;

	proc_create("tracker", S_IRUGO | S_IWUGO, NULL, &tracker_fops);
	sctable = (void *)kallsyms_lookup_name("sys_call_table");

	orig_sys_open = sctable[__NR_open];
	pte = lookup_address((unsigned long)sctable, &level);
	if (pte->pte &~ _PAGE_RW)
		pte->pte |= _PAGE_RW;
	sctable[__NR_open] = tracker_sys_open;

	return 0;
}

static void __exit tracker_exit(void) {
	unsigned int level;
	pte_t *pte;
	
	remove_proc_entry("tracker", NULL);
	sctable[__NR_open] = orig_sys_open;
	pte = lookup_address((unsigned long)sctable, &level);
	pte->pte = pte->pte &~ _PAGE_RW;
}

module_init(tracker_init);
module_exit(tracker_exit);
