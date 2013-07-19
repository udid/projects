#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/phase_shifts.h>
#include <linux/cred.h>

#include <asm/page_types.h>

#include "phase_shifts_data.h"
#include "hash_table.h"

MODULE_LICENSE("Dual BSD/GPL");

static uid_t WATCHED_USER = (uid_t) 2013;

static unsigned long HASH_SIZE = 16;
static unsigned long LOCALITY_SIZE = 64;

/** Function that extracts the page number out of an address.
 * @address - address from which we would like to extract page number.
 */
static inline unsigned long get_pn(unsigned long address)
{
	return address >> PAGE_SHIFT;
}


static struct locality_page* alloc_locality_page(unsigned long address)
{
	struct locality_page* page = (struct locality_page *) kmalloc(sizeof(struct locality_page), GFP_KERNEL);
	page->nm_page = get_pn(address);
	INIT_HLIST_NODE(&page->hash_list);
	INIT_LIST_HEAD(&page->locality_list);
	
	return page;
}
static void phase_shifts_data_init (struct phase_shift_detection_scheme* scheme)
{
	scheme->locality_hash_tbl = alloc_hash(HASH_SIZE);
	INIT_LIST_HEAD(&scheme->locality_list);
	scheme->locality_list_size = 0;
	scheme->locality_max_size = LOCALITY_SIZE;
	scheme->hash_table_size = HASH_SIZE;
	scheme->current_tick_faults = 0;
	scheme->previous_tick_faults = 0;
	spin_lock_init(&scheme->lock);
	
}
static void free_phase_shifts_data (struct phase_shift_detection_scheme* scheme)
{
	// This function also removes the locality pages themselves, thus removing the locality list.
	free_hash_list(scheme->locality_hash_tbl, scheme->hash_table_size);
	kfree(scheme);
	printk ( KERN_ALERT "phase_shifts_detector: cleaned info. \n" );
}

static void exit_callback (struct task_struct* p)
{
	char buff[TASK_COMM_LEN];
	if(p->phase_shifts_private_data)
	{
		free_phase_shifts_data(p->phase_shifts_private_data);
		p->phase_shifts_private_data = NULL;
		printk( KERN_ALERT "%s [%d]: execution has ended. \n", get_task_comm(buff, p), task_pid_nr(p) );
	}
}
static void exec_callback(struct task_struct* p)
{
	struct phase_shift_detection_scheme* data;
	char buff[TASK_COMM_LEN];
	if(task_uid(p) == WATCHED_USER)
	{
		data = (struct phase_shift_detection_scheme*) kmalloc(sizeof(struct phase_shift_detection_scheme), GFP_KERNEL);
		phase_shifts_data_init(data);
		if(p->phase_shifts_private_data)
		{
			free_phase_shifts_data(p->phase_shifts_private_data);
		}
		p->phase_shifts_private_data = (void*) data;
		printk( KERN_ALERT "%s [%d]: execution has begun. \n", get_task_comm(buff, p), task_pid_nr(p) );
	}
}

/**
 * Page fault handler callback. From where its called, its safe to assume that the user has access rights to it.
 */
static void fault_callback (struct mm_struct *mm,
		     struct vm_area_struct *vma, unsigned long address,
		     pte_t *pte, pmd_t *pmd, unsigned int flags)
{
	struct phase_shift_detection_scheme* scheme = current->phase_shifts_private_data;
	struct locality_page* page;
	pte_t entry;
	//spinlock_t* ptl;
	
	if(scheme)
	{
		entry = *pte;
		
		if(!pte_present(entry) && (vma->vm_flags & VM_EXEC)) // Page not in memory, and the memory region is executable - 1st case.
		{
			spin_lock(&scheme->lock);
			
			page = hash_find(scheme->locality_hash_tbl, scheme->hash_table_size, get_pn(address));
			if(page) // If page already in lists.
			{
				// Moves the element to the head (new references would cause it to move back to head of locality list.
				list_del(&page->locality_list);
				INIT_LIST_HEAD(&page->locality_list);
				list_add(&page->locality_list, &scheme->locality_list);
			}
			else // Otherwise, page is not already in lists.
			{
				if(scheme->locality_list_size == scheme->locality_max_size) // Checks if list is full... :(
				{
					/* In this case, needs to handle that last page is removed from list, hash table, by getting last element in list, then using list_del, hash_del.
					 * Must then make sure to flag user/supervisor bit of the removed page! Still need to figure out how to do it...
					 */
				}
				else // Can add without issues.
				{
					// Adds our new page to the list and hash table.
					page = (struct locality_page*) alloc_locality_page(address);
					list_add(&page->locality_list, &scheme->locality_list);
					hash_add(scheme->locality_hash_tbl,  scheme->hash_table_size, page, page->nm_page);
					scheme->locality_list_size++;
				}
			}
			
			scheme->current_tick_faults++;
			spin_unlock(&scheme->lock);
		
		}
		else
		{
			
		}
	}
}


/**
 * Timer callback. Tested and doesn't crash atm.
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

