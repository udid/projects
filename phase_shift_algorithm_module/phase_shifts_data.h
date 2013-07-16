/** This header defines the private data of processes for our phase shift detection algorithm. */
#ifndef _PHASE_SHIFT_PRIVATE_DATA
#define _PHASE_SHIFT_PRIVATE_DATA

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
	unsigned long current_tick_faults;
	unsigned long previous_tick_faults;
	
	/* Lock for accessing this struct. */
	spinlock_t lock; 
	
};

#endif /* _PHASE_SHIFT_PRIVATE_DATA */
