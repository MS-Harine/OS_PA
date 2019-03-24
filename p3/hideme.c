#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

struct list_head module_list;
struct list_head *target_module;

static 
int __init baremin_init(void) {
	struct list_head *ptr, *next;

	printk("Hideme : Hi.\n") ;
	
	module_list = THIS_MODULE->list;
	list_for_each_entry_safe(ptr, next, &module_list, list) {
		printk("Module list: %s\n", ptr->name);
	}

	return 0;
}

static 
void __exit baremin_exit(void) {
	printk("Hideme : Bye.\n") ;
}

module_init(baremin_init);
module_exit(baremin_exit);
