#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <list.h>

typedef int pid_t;

/* An openfile with unique file descriptor */
struct openfile{
  struct file *fileptr;
  int fd;
  struct list_elem fileelem; 
};

struct lock fslock;

void syscall_init (void);



#endif /* userprog/syscall.h */
