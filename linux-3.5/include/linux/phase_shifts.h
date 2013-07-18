#ifndef _LINUX_PHASE_SHIFT
#define _LINUX_PHASE_SHIFT

// For struct task_struct.

#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
#include <linux/mm.h>

struct phase_shift_algorithm_ops {
	/** Page fault handler callback, on handle_pte_fault() in mm/memory.c. 
	 * If user wants to edit pte, it should take a lock for it! If one wants to edit pte, check how handle_pte_fault() does it.
	 * @mm - memory descriptor of the process.
	 * @vma - virtual memory region where the address lies in.
	 * @pte - address of page table entry.
	 * @pmd - address of page table where the enrtry's in.
	 * @flags - fault flags. Interests us in the case if the operation was read or not.
	 */
	void (*fault_callback) (struct mm_struct *mm,
		     struct vm_area_struct *vma, unsigned long address,
		     pte_t *pte, pmd_t *pmd, unsigned int flags);
	
	
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
