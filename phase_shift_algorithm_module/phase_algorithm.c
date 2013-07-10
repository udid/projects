#include <linux/init.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/phase_shift.h>


/*
	struct phase_shift_algorithm_ops 
	{
		void (*handle_fault) (unsigned long);
	};
*/


MODULE_LICENSE("Dual BSD/GPL");

static void my_handle_fault(unsigned long addr)
{
	char* k = (char*) kmalloc(sizeof(char), GFP_KERNEL);
	*k = 'a';
	printk(KERN_ALERT "written %c on 0x%p\n", *k, k);
	kfree(k);
}

static int phase_shifts_init(void)
{
	struct phase_shift_algorithm_ops* ops = phase_algorithm();
	ops->handle_fault = my_handle_fault;
	printk(KERN_ALERT "device init\n");
	return 0;
}

static void phase_shifts_exit(void)
{
	struct phase_shift_algorithm_ops* ops = phase_algorithm();
	ops->handle_fault = NULL;
	printk(KERN_ALERT "device exit\n");
}

module_init(phase_shifts_init);
module_exit(phase_shifts_exit);

