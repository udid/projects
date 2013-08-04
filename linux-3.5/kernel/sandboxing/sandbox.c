#include <linux/sandbox.h>

#include <linux/kernel.h>
#include <linux/bitops.h>
#include <linux/types.h>
#include <linux/bitmap.h>
#include <linux/export.h>

#define NR_GET_PID 20
#define NUM_SANDBOXES 10

/* this is a simple sandbox for the first version,
   just to see that the limitations work */
static struct sandbox_class sandbox_array[NUM_SANDBOXES];

/* the pointer to the sandbox array is exported to the kernel */
struct sandbox_class * sandbox_list = sandbox_array;

static void init_sandbox(struct sandbox_class * sandbox) {
  if (NULL == sandbox) {
    return;
  }
  bitmap_zero(sandbox->syscalls, NUM_OF_SYSCALLS);
}

/* i currently init sandbox 1 to deny
   one system call (getpid) for testing
 */
static void init_limited_sandbox(void) {
  set_bit(NR_GET_PID, sandbox_list[1].syscalls);
}

void init_sandbox_list(void) {
  int i = 0;

  for(i=0; i < NUM_SANDBOXES; i++) {
    init_sandbox(&sandbox_list[i]);
  }
  init_limited_sandbox();
}

EXPORT_SYMBOL(sandbox_list);
