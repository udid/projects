#include <linux/phase_shifts.h>
#include <linux/export.h>

static struct phase_shift_algorithm_ops algorithm = {
	.fault_callback = NULL,
	.timer_callback = NULL,
	.fork_callback = NULL,
	.exec_callback = NULL,
	.exit_callback = NULL
};

struct phase_shift_algorithm_ops* phase_shifts_algorithm = &algorithm;

EXPORT_SYMBOL(phase_shifts_algorithm);


