#include <linux/init.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/phase_shifts.h>


/*
	struct phase_shift_algorithm_ops 
	{
		void (*handle_fault) (unsigned long);
	};
*/


MODULE_LICENSE("Dual BSD/GPL");

static void my_fault_callback(unsigned long addr)
{
	printk(KERN_ALERT "In fault callback, handle_pte_fault() .\n" );
}

static void my_timer_callback (struct task_struct* p, int user_tick)
{
	printk(KERN_ALERT "In timer callback, update_process_times() .\n" );
}

static int phase_shifts_init(void)
{
	// Init callbacks.
	phase_shifts_algorithm->fault_callback = my_fault_callback;
	phase_shifts_algorithm->timer_callback = my_timer_callback;
	
	
	printk(KERN_ALERT "Phase shifts detection algorithm activated. \n");
	return 0;
}

static void phase_shifts_exit(void)
{
	// Deactivate callbacks.
	phase_shifts_algorithm->fault_callback = NULL;
	phase_shifts_algorithm->timer_callback = NULL;
	
	
	printk(KERN_ALERT "Phase shifts detection algorithm deactivated. \n");
}

module_init(phase_shifts_init);
module_exit(phase_shifts_exit);

