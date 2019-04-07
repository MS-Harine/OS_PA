#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/unistd.h>

MODULE_LICENSE("GPL");

pid_t target_pid = -1;
void **sctable;
asmlinkage long (*orig_sys_kill)(pid_t pid, int sig);

asmlinkage long pkill_sys_kill(pid_t pid, int sig) {
	if (target_pid == pid)
		return -1;
	return orig_sys_kill(pid, sig);
}

static int pkill_proc_open(struct inode *inode, struct file *file) {
	return 0;
}

static int pkill_proc_release(struct inode *inode, struct file *file) {
	return 0;
}

static ssize_t pkill_proc_read(struct file *file, char __user *ubuf, size_t size, loff_t *offset) {
	char buf[256];
	ssize_t toread;

	sprintf(buf, "%d\n", target_pid);
	toread = strlen(buf) >= *offset + size ? size : strlen(buf) - *offset;

	if (copy_to_user(ubuf, buf + *offset, toread))
		return -EFAULT;

	*offset = *offset + toread;
	return toread;
}

static ssize_t pkill_proc_write(struct file *file, const char __user *ubuf, size_t size, loff_t *offset) {
	char buf[128];

	if (*offset != 0 || size > 128)
		return -EFAULT;

	if (copy_from_user(buf, ubuf, size))
		return -EFAULT;

	target_pid = simple_strtol(buf, NULL, 10);
	*offset = strlen(buf);
	return *offset;
}

static const struct file_operations pkill_fops = {
	.owner =	THIS_MODULE,
	.open =		pkill_proc_open,
	.read =		pkill_proc_read,
	.write =	pkill_proc_write,
	.llseek =	seq_lseek,
	.release =	pkill_proc_release,
};

static int __init pkill_init(void) {
	unsigned int level;
	pte_t *pte;

	proc_create("pkill", S_IRUGO | S_IWUGO, NULL, &pkill_fops);
	sctable = (void *)kallsyms_lookup_name("sys_call_table");
	orig_sys_kill = sctable[__NR_kill];

	pte = lookup_address((unsigned long)sctable, &level);
	if (pte->pte &~ _PAGE_RW)
		pte->pte |= _PAGE_RW;

	sctable[__NR_kill] = pkill_sys_kill;
	return 0;
}

static void __exit pkill_exit(void) {
	unsigned int level;
	pte_t *pte;
	remove_proc_entry("pkill", NULL);

	sctable[__NR_kill] = orig_sys_kill;
	pte = lookup_address((unsigned long)sctable, &level);
	pte->pte = pte->pte &~ _PAGE_RW;
}

module_init(pkill_init);
module_exit(pkill_exit);
