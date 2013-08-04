#include <linux/sandbox.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>
#include <linux/bitops.h>
#include <linux/compiler.h>

#define BLOCK_SYSCALL 1
#define SYSCALL_OK 0

SYSCALL_DEFINE0(get_sandbox_id)
{
  return current->sandbox_id;
}

SYSCALL_DEFINE1(switch_sandbox, unsigned long, sandbox_id)
{
  if (0 != current->sandbox_id) {
    printk(KERN_ALERT "switch_sandbox() is available only to sandbox 0 processes.\n");
    return -EINVAL;
  }
  current->sandbox_id = sandbox_id;
  return 0;
}

asmlinkage int sandbox_block_syscall(int syscall_num) {
  if (unlikely(test_bit(syscall_num, current_sandbox.syscalls))) {
    printk("bit %d is set in current sandbox (%ld)\n", syscall_num, current->sandbox_id);
    return BLOCK_SYSCALL;
  }

  return SYSCALL_OK;
}
