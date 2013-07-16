#ifndef _LINUX_PHASE_SHIFT
#define _LINUX_PHASE_SHIFT

// For struct task_struct.

#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/sched.h>

struct phase_shift_algorithm_ops {
	// Function will be called from the page fault handler.
	void (*fault_callback) (unsigned long);
	
	
	/** timer_callback is called by update_process_times() in kernel/timer.c. 
	 * Detect phase shifts, update page fault counters.
	 * @p - Process to examine its previous and current page fault number of ticks.
	 * @user_tick - If 1, tick was user time, 0 for system time. 
	 */ 
	void (*timer_callback) (struct task_struct* p, int user_tick);
	
	/** Callback on copy_process on fork. 
	 * @orig - original process that was copied. 
	 * @new - new process that was forked.
	 * @clone_flags - clone flags - if it was CLONED_VM or not.
	 */
	 void (*fork_callback) (struct task_struct* orig, struct task_struct* new, unsigned long clone_flags);
	 
	 /** Callback on do_execve_common on fork. Should empty the list.
	 * @p - process that was exec'd.
	 */
	 void (*exec_callback) (struct task_struct* p);
	 
	 /** Callback when a process is destroyed. Should clean memory saved in task_struct for our algorithm.
	  * Important to note - do not leave processes that were created with this module alive 
	  * @p - process that is to be destroyed.
	  */
	  void (*exit_callback) (struct task_struct* p);
};

extern struct phase_shift_algorithm_ops* phase_shifts_algorithm;

#endif /* _LINUX_PHASE_SHIFT */
