#ifndef _LINUX_PHASE_SHIFT
#define _LINUX_PHASE_SHIFT

// For struct task_struct.

#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/sched.h>


// Page in a locality list. It is also hashable,  
struct locality_page {
	// Page number (on 32-bit systems, should be the 20 most significant bits.
	unsigned long nm_page;
	// Linked list of pages to be used as the locality list, and a hash table 
	struct list_head locality_list;
	struct hlist_node hash_list;
};

struct phase_shift_detection_scheme {
	// Linked list of its locality pages and a hash table of locality pages.
	struct list_head locality_list;
	struct hlist_head* locality_hash_tbl;
	// Size of locality list, and hash table size (Not the amount of items in hash table, but rather the size of hash table array).
	unsigned long hash_table_size;
	unsigned long locality_list_size;
	unsigned long locality_max_size;
	
	// Counters to save current tick and previous tick faults.
	unsigned long current_tick_faults;
	unsigned long previous_tick_faults;
	
	// Lock for accessing this struct.
	spinlock_t lock; 
	
};

struct phase_shift_algorithm_ops {
	// Function will be called from the page fault handler.
	void (*fault_callback) (unsigned long);
	
	
	/** timer_callback is called by update_process_times() in kernel/timer.c. 
	 * Detect phase shifts, update page fault counters.
	 * @p - Process to examine its previous and current page fault number of ticks.
	 * @user_tick - If 1, tick was user time, 0 for system time. 
	 */ 
	void (*timer_callback) (struct task_struct* p, int user_tick);
	
	/** init_phase_shift_scheme_callback callback is used to allocate phase shift detection schemes. For instance, its job is to initialize the spinlock, lists, hash table, sizes and such.
	 * @pointer to private data.
	 */
	void (*init_phase_shift_scheme_callback) (void* scheme_private_data);
	
	/** Callback on copy_process on fork. Accepts 
	 * @orig - original process that was copied. 
	 * @new - new process that was forked.
	 * @clone_flags - clone flags - if it was CLONED_VM or not.
	 */
	 void (*fork_callback) (struct task_struct* orig, struct task_struct* new, unsigned long clone_flags);
	 
	 /** Callback on do_execve_common on fork. Should empty the list.
	 * @p - process that was exec'd.
	 */
	 void (*exec_callback) (struct task_struct* p);
	 
	 /** Callback when a process is destroyed, so that 
	  * @p - process that is to be destroyed.
	  */
	  void (*exit_callback) (struct task_struct* p);
};

extern struct phase_shift_algorithm_ops* phase_shifts_algorithm;

#endif /* _LINUX_PHASE_SHIFT */
