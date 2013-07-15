#include <linux/init.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/phase_shifts.h>

MODULE_LICENSE("Dual BSD/GPL");

static void dummy_fault_callback (unsigned long addr)
{
	printk(KERN_ALERT "In fault callback, handle_pte_fault() .\n" );
}
static void dummy_timer_callback(struct task_struct* p, int user_tick)
{
	printk (KERN_ALERT "In timer callback, by %d\n", task_pid_nr(p));
}
static void dummy_fork_callback (struct task_struct* orig, struct task_struct* new, unsigned long clone_flags)
{
	printk (KERN_ALERT "In fork callback, parent:[%d], son:[%d]\n", task_pid_nr(orig), task_pid_nr(new));
}
static void dummy_exit_callback (struct task_struct* p)
{
	printk(KERN_ALERT "process [%d] is destroyed. \n" , task_pid_nr(p));
}

/*static void detect_shift_timer_callback (struct task_struct* p, int user_tick)
{	
	struct phase_shift_detection_scheme* scheme = p->pshift_scheme;
	int detected = 0;
	// Check if a scheme on this process is well defined, and tick was user time.
	if(scheme && user_tick)
	{
		* 
		 * Locking the phase shift detection scheme associated with current.
		 *
		spin_lock(&scheme->lock); 
		
		// Check for a tick - if previous tick had no faults while current one did.
		if(!(scheme->previous_tick_faults) && scheme->current_tick_faults > 0)
		{
			// If so, sets a flag to indicate a detection.
			detected = 1;
		}
		
		// Resetting counters. 
		scheme->previous_tick_faults = scheme->current_tick_faults;
		scheme->current_tick_faults = 0;
		
		
		spin_unlock(&scheme->unlock);
		
		// After exiting critical section, can safely print of a shift detection.
		if(detected)
		{
			printk( KERN_ALERT "Phase shift detected for process %d\n", task_pid_nr(p) );
		}
	}
}*/

static int phase_shifts_init(void)
{
	// Init callbacks.
	phase_shifts_algorithm->exit_callback = dummy_exit_callback;
	
	printk(KERN_ALERT "Phase shifts detection algorithm activated. \n");
	return 0;
}

static void phase_shifts_exit(void)
{
	// Deactivate callbacks.
	phase_shifts_algorithm->exit_callback = dummy_exit_callback;
	
	
	printk(KERN_ALERT "Phase shifts detection algorithm deactivated. \n");
}

module_init(phase_shifts_init);
module_exit(phase_shifts_exit);

