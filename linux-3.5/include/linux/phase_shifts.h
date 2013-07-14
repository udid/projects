#ifndef _LINUX_PHASE_SHIFT
#define _LINUX_PHASE_SHIFT

// For struct task_struct.
#include <linux/sched.h>

struct phase_shift_algorithm_ops 
{
	// Function will be called from the page fault handler.
	void (*fault_callback) (unsigned long);
	
	
	/* timer_callback is called by update_process_times() in kernel/timer.c. 
	 * Detect phase shifts, update page fault counters.
	 * @p - Process to examine its previous and current page fault number of ticks.
	 * @user_tick - If 1, tick was user time, 0 for system time. 
	 */ 
	void (*timer_callback) (struct task_struct* p, int user_tick);
};

extern struct phase_shift_algorithm_ops* phase_shifts_algorithm;

#endif /* _LINUX_PHASE_SHIFT */
