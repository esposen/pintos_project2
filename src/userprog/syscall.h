#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <list.h>
#include "threads/synch.h"

#define NOT_LOADED 0
#define LOAD_SUCCESS 1
#define LOAD_FAIL 2

typedef int pid_t;

struct openfile{
  struct file *fileptr;
  int fd;
  struct list_elem fileelem; 
};

void syscall_init (void);

#endif /* userprog/syscall.h */
