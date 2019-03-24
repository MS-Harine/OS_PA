#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

static 
int __init baremin_init(void) {
	struct module *target_module;
	printk("Hideme : Hi.\n") ;
	
	list_for_each_entry(target_module, &THIS_MODULE->list, list) {
		printk("Module list: %s\n", target_module->name);
	}

	return 0;
}

static 
void __exit baremin_exit(void) {
	printk("Hideme : Bye.\n") ;
}

module_init(baremin_init);
module_exit(baremin_exit);
