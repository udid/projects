#ifndef _LINUX_PHASE_SHIFT
#define _LINUX_PHASE_SHIFT

#include <asm/pgalloc.h>

struct phase_shift_algorithm_ops 
{
	void (*handle_fault) (unsigned long);
};

extern struct phase_shift_algorithm_ops* phase_algorithm(void);

#endif /* _LINUX_PHASE_SHIFT */
