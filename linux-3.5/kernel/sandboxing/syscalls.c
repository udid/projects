#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE1(switch_sandbox, unsigned long, sandbox_id)
{
  printk("called sys_switch_sandbox(%ld)\n", sandbox_id);
  if (0 != current->sandbox_id) {
    printk(KERN_ALERT "switch_sandbox() is available only to sandbox 0 processes.\n");
    return -EINVAL;
  }
  current->sandbox_id = sandbox_id;
  return 0;
}
