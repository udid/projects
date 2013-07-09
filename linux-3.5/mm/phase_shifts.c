#include <linux/phase_shift.h>
#include <linux/export.h>

static struct phase_shift_algorithm_ops alg_ops = {
	.handle_fault = NULL
};

extern struct phase_shift_algorithm_ops* phase_algorithm(void)
{
	return &alg_ops;
}

EXPORT_SYMBOL(phase_algorithm);


