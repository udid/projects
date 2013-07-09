#include <linux/init.h>
#include <linux/module.h>

#include <linux/fs.h>
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
	printk(KERN_ALERT "fault 2 detected at : %p\n", (void*)addr);
}

static int device_init(void)
{
	struct phase_shift_algorithm_ops* ops = phase_algorithm();
	ops->handle_fault = my_handle_fault;
	printk(KERN_ALERT "device init\n");
	return 0;
}

static void device_exit(void)
{
	struct phase_shift_algorithm_ops* ops = phase_algorithm();
	ops->handle_fault = NULL;
	printk(KERN_ALERT "device exit\n");
}

module_init(device_init);
module_exit(device_exit);

