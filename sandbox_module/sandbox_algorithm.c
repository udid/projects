#include <linux/init.h>
#include <linux/module.h>
#include <linux/export.h>

#include <linux/kernel.h>
#include <linux/syscalls.h>

#include <linux/fs.h>
#include <linux/fdtable.h>

#include <linux/types.h>
#include <linux/string.h>
#include <linux/bitops.h>
#include <linux/bitmap.h>

#include <linux/sandbox.h>
#include "sandbox_algorithm.h"

MODULE_LICENSE("Dual BSD/GPL");

#define current_sandbox (sandbox_list[current->sandbox_id])

/*******************************************************************************
  module private data
*******************************************************************************/
/* this array holds each sandbox configuration */
static struct sandbox_class sandbox_array[NUM_SANDBOXES];
struct sandbox_class * sandbox_list = sandbox_array;

/*******************************************************************************
  kernel operations
*******************************************************************************/
static void _chroot_jail(const char __user * new_root)
{
  sys_chdir(new_root);
  sys_chroot(new_root);
}

void _strip_files(void)
{
  struct file * filp;
  struct fdtable *fdt;
  unsigned int i;
  
  fdt = files_fdtable(current->files);
  for (i = 0; i < fdt->max_fds; i++) {
    filp = fdt->fd[i];
    if (filp) {
      sys_close(i);
    }
  }
}

/*******************************************************************************
  kernel hooks
*******************************************************************************/
static int sandbox_enter(unsigned long sandbox_id)
{
  struct sandbox_class * sandbox = &sandbox_list[sandbox_id];
  
  if (sandbox->strip_files) {
    _strip_files();
  }

  /* trap the process in a chroot jail */
  if (NULL != sandbox->fs_root) {
    _chroot_jail(sandbox->fs_root);
  }

  /* then we change the sandbox id in the task struct.
     that is because after changing the sandbox id many
     os operations are blocked by the sandbox hooks. 
  */
  task_lock(current);
  current->sandbox_id = sandbox_id;
  task_unlock(current);

  return 0;
}

static int sandbox_syscall(int syscall_num)
{
  if (unlikely(test_bit(syscall_num, current_sandbox.syscalls))) {
    printk("bit %d is set in current sandbox (%ld)\n", syscall_num, current->sandbox_id);
    return BLOCK_SYSCALL;
  }
  return SYSCALL_OK;
}

/*******************************************************************************
  sandbox administration (exported to char device module)
*******************************************************************************/
static struct sandbox_class * get_sandbox(unsigned long sandbox_id)
{
  if (NUM_SANDBOXES <= sandbox_id) {
    return NULL;
  }
  return &sandbox_list[sandbox_id];
}

static void sandbox_set_jail(struct sandbox_class * sandbox, char * jail_dir)
{
  sandbox->fs_root = jail_dir;
}

static void sandbox_set_strip_files(struct sandbox_class * sandbox, bool strip_files)
{
  sandbox->strip_files = strip_files;
}

static void sandbox_set_syscall_bit(struct sandbox_class * sandbox, unsigned int syscall_num, bool bitval)
{
  if (bitval) {
    set_bit(syscall_num, sandbox->syscalls);
  } else {
    clear_bit(syscall_num, sandbox->syscalls);
  }
}

/* init_sandbox() should load the data structure
   with the default and most permissive configuration */
static void init_sandbox(struct sandbox_class * sandbox) {
  sandbox->fs_root = NULL;
  sandbox->file_list = NULL;
  sandbox->ip_list = NULL;

  sandbox->strip_files = 0;
  sandbox->disallow_files_by_default = 0;
  sandbox->disallow_ips_by_default = 0;
  
  bitmap_zero(sandbox->syscalls, NUM_OF_SYSCALLS);
}

/* i currently init sandbox number 1 to deny 
   one system call (getpid) for testing */ 
#define BLOCKED_SYSCALL (20)
static void init_limited_sandbox(struct sandbox_class * sandbox) 
{ 
  sandbox_set_jail(sandbox, "/var/jail/");
  sandbox_set_strip_files(sandbox, true);
  sandbox_set_syscall_bit(sandbox, BLOCKED_SYSCALL, true);
} 

void init_sandbox_list(void) {
  int i = 0;

  for(i=0; i < NUM_SANDBOXES; i++) {
    init_sandbox(get_sandbox(i));
  }
  /* temp */
  init_limited_sandbox(get_sandbox(1));
}

/*******************************************************************************
  module init and exit
*******************************************************************************/
static int __init sandbox_init(void)
{
  init_sandbox_list();

  sandbox_algorithm->enter_callback = sandbox_enter;
  sandbox_algorithm->syscall_callback = sandbox_syscall;
  printk(KERN_ALERT "sandbox module loaded.\n");
  return 0;
}


static void __exit sandbox_exit(void)
{
  sandbox_algorithm->enter_callback = NULL;
  sandbox_algorithm->syscall_callback = NULL;
  printk(KERN_ALERT "sandbox module unloaded.\n");
}

module_init(sandbox_init);
module_exit(sandbox_exit);
