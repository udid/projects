#include <asm/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/compiler.h>

asmlinkage long sys_fork_into_sandbox(unsigned long sandbox_id,
				      struct pt_regs *regs)
{
  printk("called sys_fork_into_sandbox(%ld)\n", sandbox_id);
  return do_fork(SIGCHLD, regs->sp, regs, 0, NULL, NULL);
		 /* sandbox_id); */
}
