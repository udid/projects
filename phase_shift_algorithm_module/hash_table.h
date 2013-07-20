#ifndef __PHASE_SHIFTS_HASH_TABLE
#define __PHASE_SHIFTS_HASH_TABLE

#include <linux/list.h>
#include <linux/hash.h>
#include <linux/slab.h>
#include <linux/log2.h>

/** Hash table allocation.
 * @hash_size - must be a power of 2!
 */
static inline struct hlist_head* alloc_hash(unsigned long hash_size)
{
	unsigned long i;
	struct hlist_head* hash_tbl = (struct hlist_head*) kmalloc ( hash_size* sizeof(struct hlist_head), GFP_KERNEL);
	for(i = 0UL; i < hash_size; i++)
	{
		INIT_HLIST_HEAD(&hash_tbl[i]);
	}
	return hash_tbl;
}

/** Adding a locality_page* to the hash table.
 * @hash_tbl - the hast table to be added to.
 * @hash_size - the hash table's size.
 * @page - the struct locality_page to be added.
 * @key - the key that is used in the hash table.
 */ 
static inline void hash_add(struct hlist_head* hash_tbl, unsigned long hash_size, struct locality_page* page, unsigned long key)
{
	INIT_HLIST_NODE(&page->hash_list);
	hlist_add_head(&page->hash_list, &hash_tbl[hash_long(key, ilog2(hash_size))]);
} 

/** Looks for an element of locality_page* with the given key. Returns NULL such one doesn't exist.
 * @hash_tbl - hash table.
 * @hash_size - hash table's size.
 * @key - the key used to search.
 */ 
static inline struct locality_page* hash_find(struct hlist_head* hash_tbl, unsigned long hash_size, unsigned long key)
{
	struct hlist_head* head = &hash_tbl[hash_long(key, ilog2(hash_size))];
	struct locality_page* page;
	struct hlist_node* ptr;
	hlist_for_each_entry(page, ptr, head, hash_list)
	{
		if(page->nm_page == key)
			return page;
	}
	return NULL;
}
/** Removes a page from its hash table.
 * @page - page that is being removed from the hash table.
 */ 
static inline void hash_del(struct locality_page* page)
{
	hlist_del(&page->hash_list);
}
/** Frees the memory used by the hash table, and frees all struct locality_page* structures. 
 ** It also removes each of the struct locality_page* from list of locality, thus also removing the list.
 * @hash_tbl - hash table.
 * @hash_size - the hash table's size.
 */ 
static inline void free_hash_list(struct hlist_head* hash_tbl, unsigned long hash_size)
{
	//unsigned long i;
	//struct hlist_node* tmp;
	//struct hlist_node* ptr;
	//struct locality_page* page;
	//for(i = 0UL; i < hash_size; i++)
	//{
		//hlist_for_each_entry_safe(page, ptr, tmp, &hash_tbl[i], hash_list)
		//{
			//hlist_del(&page->hash_list);
			//list_del(&page->locality_list);
		//}
	//}
	kfree(hash_tbl);
}




#endif
