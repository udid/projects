#include <linux/module.h>
#include "goldenblock.h"

#define SECTOR_SIZE 512

static int goldenblock_open(struct block_device* bdev, fmode_t mode)
{
	GoldenBlock* block = bdev->bd_disk->private_data;
	
	printk(KERN_ALERT "goldenblock_open(): Called");

	if (!uid_eq(current_uid(), block->owner_uid))
	{
		printk(KERN_ALERT "goldenblock_open(): Permission denied. Opener is not the owner");
		return -1;
	}

	return 0;
}

static int goldenblock_release(struct gendisk* gd, fmode_t mode)
{
	printk(KERN_ALERT "goldenblock_release(): Called");
	return 0;
}

void goldenblock_request(struct request_queue* queue)
{
	
}

static const struct block_device_operations goldenblock_ops = {
	.owner = THIS_MODULE,
	.open = goldenblock_open,
	.release = goldenblock_release
};

void golden_block_list_initialize(GoldenBlock* block)
{
	INIT_LIST_HEAD(&(block->list));
}

int golden_block_create(char* name, int capacity, int minors, GoldenBlock** out)
{
	int err = 0;
	GoldenBlock* block = kzalloc(sizeof(GoldenBlock), GFP_KERNEL);

	printk(KERN_ALERT "golden_block_create(): Starting");

	if (block == NULL)
	{
		err = -ENOMEM;
		goto end;
	}

	block->owner_uid = current_uid();
	block->refcount = 0;

	block->major = register_blkdev(0, name);
	if (block->major < 0)
	{
		err = -ENOMEM;
		goto end;
	}

	printk(KERN_ALERT "golden_block_create(): registered block device");
	spin_lock_init(&(block->lock));
	
	block->gd = alloc_disk(minors);

	if (block->gd == NULL)
	{
		err = -ENOMEM;
		goto end;
	}

	block->gd->first_minor = 1;
	block->gd->fops = &goldenblock_ops;
    block->gd->private_data = block;
    strcpy(block->gd->disk_name, name);
    set_capacity(block->gd, capacity * SECTOR_SIZE);

    block->gd->queue = blk_init_queue(goldenblock_request, &block->lock);
    if (block->gd->queue == NULL)
	{
		err = -ENOMEM;
	    goto end;
	}
	
	printk(KERN_ALERT "golden_block_create(): About to add_disk");

	add_disk(block->gd);
	*out = block;
end:
	if (err < 0)
	{
		if (block->gd != NULL)
			del_gendisk(block->gd);
		if (block != NULL);
			kfree(block);
	}

	return err;	
}


