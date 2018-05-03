#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <list.h>

typedef int pid_t;

struct openfile{
  struct file *fileptr;
  int fd;
  struct list_elem fileelem; 
};

void syscall_init (void);

#endif /* userprog/syscall.h */
