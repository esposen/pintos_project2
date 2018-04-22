#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "lib/string.h"
#include "lib/stdio.h"
#include "threads/malloc.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "lib/debug.h"

static void syscall_handler (struct intr_frame *);

void s_halt (void) NO_RETURN;
void s_exit (int status) NO_RETURN;
pid_t s_exec (const char *file);
int s_wait (pid_t);
bool s_create (const char *file, unsigned initial_size);
bool s_remove (const char *file);
int s_open (const char *file);
int s_filesize (int fd);
int s_read (int fd, void *buffer, unsigned length);
int s_write(int fd, const void *buffer, unsigned size);
void s_seek (int fd, unsigned position);
unsigned s_tell (int fd);
void s_close (int fd);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  void *sp = f->esp;

  //hex_dump(sp,sp,PHYS_BASE-sp,true); printf("\n");
  int syscode = *(int *)sp;
  sp += sizeof(int *);
  //syscode = SYS_HALT;
  printf("System Call: %d\n",syscode);

  switch (syscode){
  case SYS_HALT: ;
    s_halt();
    break;
  case SYS_EXIT: ;
    break;
  case SYS_EXEC: ;
    break;
  case SYS_WAIT: ;
    break;
  case SYS_CREATE: ;
    break;
  case SYS_REMOVE: ;
    break;
  case SYS_OPEN: ;
    break;
  case SYS_FILESIZE: ;
    break;
  case SYS_READ: ;
    break;
  case SYS_WRITE: ;//empty statement
    int fd=*(int *)sp;
    sp += sizeof(int *);
    s_write(fd,sp,1);
    break;
  case SYS_SEEK : ;
    break;
  case SYS_TELL: ;
    break;
  case SYS_CLOSE: ;
    break;
    
  default:
    printf("syscode not found");
    break;
  }
  
  thread_exit ();
}

void s_halt (void){
  printf("halt\n");
  shutdown_power_off();
  NOT_REACHED();
}

// void s_exit (int status);
// pid_t s_exec (const char *file);
// int s_wait (pid_t);
// bool s_create (const char *file, unsigned initial_size);
// bool s_remove (const char *file);
// int s_open (const char *file);
// int filesize (int fd);
// int read (int fd, void *buffer, unsigned length);
int s_write(int fd, const void *buffer, unsigned size){
  printf("SYS_WRITE - fd: %d, buffer: %s\n",fd,"nothing");
  return 1; 
}
// void seek (int fd, unsigned position);
// unsigned tell (int fd);
// void close (int fd);
