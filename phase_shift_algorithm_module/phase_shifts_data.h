/** This header defines the private data of processes for our phase shift detection algorithm. */
#ifndef _PHASE_SHIFT_PRIVATE_DATA
#define _PHASE_SHIFT_PRIVATE_DATA

#include <linux/list.h>
#include <linux/spinlock.h>
#include <asm/atomic.h>

// Page in a locality list. It is also hashable,  
struct locality_page {
	// Page number (on 32-bit systems, should be the 20 most significant bits.
	unsigned long nm_page;
	// Linked list of pages to be used as the locality list, and a hash table 
	struct list_head locality_list;
	struct hlist_node hash_list;
};

struct phase_shift_detection_scheme {
	/* Linked list of its locality pages and a hash table of locality pages. */
	struct list_head locality_list;
	struct hlist_head* locality_hash_tbl;
	/* Size of locality list, and hash table size (Not the amount of items in hash table, but rather the size of hash table array). */
	unsigned long hash_table_size;
	unsigned long locality_list_size;
	unsigned long locality_max_size;
	
	/* Counters to save current tick and previous tick faults. */
	atomic_t current_tick_faults;
	int previous_tick_faults;
	
	/* Pool of locality page structs. This is used so we won't need to allocate in page fault handler anything. */
	struct locality_page* pool;
	/* Index where the page is free to use. When pool is full, it is promised that the handler won't try to use free_index, as the list is full thus it will use
	 * A previously allocated page.
	 */
	unsigned long free_index; 
	
	unsigned long number_of_ticks;
	
};

#endif /* _PHASE_SHIFT_PRIVATE_DATA */
