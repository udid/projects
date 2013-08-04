#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#define FORK (2)
#define SYS_GET_PID (20)
#define SYS_SWITCH_SANDBOX (350)
#define SYS_GET_SANDBOX (351)

/* getpid */
#include <sys/types.h>
#include <unistd.h>

#include <syslog.h>

int getpid_syscall(void) {
  return (int) syscall(SYS_GET_PID);
}

int main(void) 
{
  int prio = LOG_USER | LOG_ALERT;
  int ret, pid;
  long new_sandbox = 1;
  pid_t father, child;
  printf("calling sys_switch_sandbox...\n");

  pid = (int) syscall(FORK);
  if(pid == 0) {
    /* this is the child */
    syslog(prio, "sandbox: Papa!! (child)\n");
    /* switches sandbox */
    ret = (int) syscall(SYS_SWITCH_SANDBOX, new_sandbox);
    if (0 != ret) {
      printf("switch_sandbox failed\n");
    }

    new_sandbox = (long) syscall(SYS_GET_SANDBOX);
    syslog(prio, "new sandbox is %ld\n", new_sandbox);
      
    father = getppid();
    syslog(prio, "sandbox: child ppid is %d\n", (int) father);
    child = getpid_syscall();
    syslog(prio, "sandbox: child pid is %d\n", (int) child);
  } else {
    /* this is the father */
    syslog(prio, "sandbox: Gingie!! (father, pid of child is: %d)\n", pid);
    father = getppid();
    syslog(prio, "sandbox: father ppid is %d\n", (int) father);
    child = getpid_syscall();
    syslog(prio, "sandbox: father pid is %d\n", (int) child);
  }
  return 0;
}
