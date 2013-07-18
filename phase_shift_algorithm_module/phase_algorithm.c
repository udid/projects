#include <linux/init.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/phase_shifts.h>

#include "phase_shifts_data.h"

MODULE_LICENSE("Dual BSD/GPL");

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
static void dummy_exec_callback (struct task_struct* p)
{
	printk(KERN_ALERT "process [%d] has conducted exec. private data %p \n", task_pid_nr(p), p->phase_shifts_private_data);
}

static void exit_callback (struct task_struct* p)
{
	if(p->phase_shifts_private_data)
	{
		kfree(p->phase_shifts_private_data);
		p->phase_shifts_private_data = NULL;
	}
}
static void exec_callback(struct task_struct* p)
{
	struct phase_shift_detection_scheme* data = 
		(struct phase_shift_detection_scheme*) kmalloc(sizeof(struct phase_shift_detection_scheme), GFP_KERNEL);
	
	data->current_tick_faults = 0;
	data->previous_tick_faults = 0;
	spin_lock_init(&data->lock);
	if(p->phase_shifts_private_data)
	{
		kfree(p->phase_shifts_private_data);
	}
	p->phase_shifts_private_data = (void*) data;
}

/**
 * Page fault handler callback. From where its called, its safe to assume that the user has access rights to it.
 */
static void fault_callback (struct mm_struct *mm,
		     struct vm_area_struct *vma, unsigned long address,
		     pte_t *pte, pmd_t *pmd, unsigned int flags)
{
	struct phase_shift_detection_scheme* scheme = current->phase_shifts_private_data;
	pte_t entry;
	//spinlock_t* ptl;
	
	if(scheme)
	{
		entry = *pte;
		if(!pte_present(entry))
		{
			if(vma->vm_flags & VM_EXEC)
			{
				spin_lock(&scheme->lock);
				scheme->current_tick_faults++;
				spin_unlock(&scheme->lock);
			}
		}
	}
}


/**
 * Timer callback - older version which should still be updated. Not tested yet!
 */
static void timer_callback (struct task_struct* p, int user_tick)
{	
	struct phase_shift_detection_scheme* scheme = (struct phase_shift_detection_scheme*) p->phase_shifts_private_data;
	int detected = 0;
	char buff[TASK_COMM_LEN];
	// Check if a scheme on this process is well defined, and tick was user time.
	if(scheme && user_tick)
	{
		/* 
		 * Locking the phase shift detection scheme associated with current.
		 */
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
		
		
		spin_unlock(&scheme->lock);
		
		// After exiting critical section, can safely print of a shift detection.
		
		if(detected)
		{
			printk( KERN_ALERT "%s [%d]: phase shift detected. \n", get_task_comm(buff, p), task_pid_nr(p) );
		}
	}
}

static int phase_shifts_init(void)
{
	// Init callbacks.
	phase_shifts_algorithm->exit_callback = exit_callback;
	phase_shifts_algorithm->exec_callback = exec_callback;
	phase_shifts_algorithm->fault_callback = fault_callback;
	phase_shifts_algorithm->timer_callback = timer_callback;
	
	printk(KERN_ALERT "Phase shifts detection algorithm activated. \n");
	return 0;
}

static void phase_shifts_exit(void)
{
	// Deactivate callbacks.
	phase_shifts_algorithm->exit_callback = NULL;
	phase_shifts_algorithm->exec_callback = NULL;
	phase_shifts_algorithm->fault_callback = NULL;
	phase_shifts_algorithm->timer_callback = NULL;
	
	
	printk(KERN_ALERT "Phase shifts detection algorithm deactivated. \n");
}

module_init(phase_shifts_init);
module_exit(phase_shifts_exit);

