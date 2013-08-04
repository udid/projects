#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#define FORK (2)
#define SB_FORK (350)

/* getpid */
#include <sys/types.h>
#include <unistd.h>

#include <syslog.h>

int main(void) 
{
  int prio = LOG_USER | LOG_ALERT;
  int pid = 0;
  pid_t father, child;
  printf("calling sys_fork_into_sandbox...\n");

  pid = (int) syscall(SB_FORK, 1); 
  /* pid = (int) syscall(FORK); */
  if(pid == 0) {
    /* this is the child */
    syslog(prio, "sandbox: Papa!!\n");
    father = getppid();
    syslog(prio, "sandbox: father pid is %d\n", (int) father);
    child = getpid();
    syslog(prio, "sandbox: child pid is %d\n", (int) child);
  } else {
    /* this is the father */
    syslog(prio, "sandbox: Gingie!! (num: %d)\n", pid);
    father = getppid();
    syslog(prio, "sandbox: father pid is %d\n", (int) father);
    child = getpid();
    syslog(prio, "sandbox: child pid is %d\n", (int) child);
  }
  return 0;
}
