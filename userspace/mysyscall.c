#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>

/* for invoking syscalls */
#include <sys/syscall.h>

/* getpid */
#include <sys/types.h>
#include <unistd.h>

/* for logging */
#include <syslog.h>

/* open */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* strnlen */
#include <string.h>

/* constants */
#define FORK (2)
#define SYS_GET_PID (20)
#define SYS_SWITCH_SANDBOX (350)
#define SYS_GET_SANDBOX (351)
#define MAX_LENGTH (100)

int getpid_syscall(void) {
  return (int) syscall(SYS_GET_PID);
}

int before_fork(void) 
{
  int prio = LOG_USER | LOG_ALERT;
  int fd_common;
  ssize_t bw = 0;
  char * common_str = "common";
  ssize_t cstr_size = strnlen(common_str, MAX_LENGTH);

  fd_common = open("common.txt", O_CREAT | O_RDWR, S_IRWXU);
  bw = write(fd_common, common_str, cstr_size);
  syslog(prio, "sandbox-common: written %d bytes to fd %d\n", (int) bw, fd_common);

  return fd_common;
}

void after_fork(char * identity, int fd_common)
{
  int prio = LOG_USER | LOG_ALERT;
  pid_t pid;
  ssize_t br, bw;
  char * str2 = " 2ndstr";
  ssize_t str2_size = strnlen(str2, MAX_LENGTH);
  char read_buffer[MAX_LENGTH];
  int cc, co;

  int fd_outsider;
  
  /* getppid */
  pid = getppid();
  syslog(prio, "%s: ppid is %d\n", identity, (int) pid);

  /* getpid */
  pid = getpid_syscall();
  syslog(prio, "%s: pid is %d\n", identity, (int) pid);

  /* trying to keep writing to fd_common */
  /* only father should succeed */
  bw = write(fd_common, str2, str2_size);
  syslog(prio, "%s: written %d bytes to fd %d\n", identity, (int) bw, fd_common);

  /* trying to open file outside sandbox */
  /* only father should succeed */
  fd_outsider = open("existing.txt", O_RDONLY, S_IRWXU);

  br = read(fd_outsider, read_buffer, MAX_LENGTH);
  syslog(prio, "%s: read %d bytes from fd %d: [%s]\n", identity, (int) br, fd_outsider, read_buffer);

  cc = close(fd_common);
  syslog(prio, "%s: close fd common %d returned %d\n", identity, fd_common, cc);
  
  co = close(fd_outsider);
  syslog(prio, "%s: close fd outsider %d returned %d\n", identity, fd_outsider, co);
}

int main(void) 
{
  int prio = LOG_USER | LOG_ALERT;
  int ret, pid;
  long new_sandbox = 1;
  int fd_common;
  
  fd_common = before_fork();

  printf("starting sandbox test program... check syslog.\n");
  
  pid = (int) syscall(FORK);
  if(pid == 0) {
    /* this is the child */
    syslog(prio, "sandbox-child: fork() returned!\n");

    /* switches sandbox */
    ret = (int) syscall(SYS_SWITCH_SANDBOX, new_sandbox);
    syslog(prio, "sandbox-child: switch_sandbox() ret: %d\n", ret);

    new_sandbox = (long) syscall(SYS_GET_SANDBOX);
    syslog(prio, "sandbox-child: get_sandbox() ret: %ld\n", new_sandbox);

    /* same actions */
    after_fork("sandbox-child", fd_common);
  
  } else {
    /* this is the father */
    syslog(prio, "sandbox-father: (pid of child is %d)\n", pid);
    
    /* same actions */
    after_fork("sandbox-father", fd_common);
  }
  return 0;
}
