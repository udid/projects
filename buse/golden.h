#ifndef __GOLDEN_H__
#define __GOLDEN_H__

#include <linux/semaphore.h>
#include "goldenchar.h"
#include "goldenblock.h"

typedef struct GoldenGate{
	GoldenChar gchar;
	GoldenBlock gblock_list_head;
	struct semaphore golden_lock;
} GoldenGate;

#endif
