#ifndef _LINUX_SANDBOX
#define _LINUX_SANDBOX

#include <linux/kernel.h>
#include <linux/bitops.h>
#include <linux/types.h>

#define current_sandbox (sandbox_list[current->sandbox_id])

/* temp */
#define NUM_OF_SYSCALLS 350

struct sandbox_class {
  /* 
     this bitmap disallows certain syscalls from running.
     if the bit syscalls[SYSCALL_NUM] == 1 then 
     the system call identified by SYSCALL_NUM is disabled.
  */
  DECLARE_BITMAP(syscalls, NUM_OF_SYSCALLS);
};

/* exported to the kernel */
extern struct sandbox_class * sandbox_list;
extern void init_sandbox_list(void);
extern asmlinkage int sandbox_block_syscall(int syscall_num);

#endif /* _LINUX_SANDBOX */
