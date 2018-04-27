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
#include "pagedir.h"

static void syscall_handler (struct intr_frame *);

void s_halt (void) NO_RETURN;
void s_exit (void) NO_RETURN;
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

//Helper Function
uint32_t *get_args(void **args);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  void *sp = f->esp; 
  // hex_dump(f->esp,f->esp,PHYS_BASE-f->esp,true); printf("\n");
  
  int syscode = *(int *)sp;
  sp += sizeof(int *);
  //printf("System Call: %d\n",syscode);
  uint32_t args[3];

  switch (syscode){
  case SYS_HALT: ;
    s_halt();
    break;
  case SYS_EXIT: ;
    int status = *(int *)sp; //grab status and store in eax register
    f->eax = status;
    printf("%s: exit(%d)\n",thread_current()->name,f->eax);
    s_exit();
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
    
  case SYS_WRITE: ;
    for(int i=0;i<3;i++){
      args[i] = *(int *)sp;
      sp += sizeof(int *);
    }
    //args[1] = pagedir_get_page(thread_current()->pagedir,(const void *) args[1]);
    int retval = s_write((int)args[0],(void *)args[1],(unsigned)args[2]);
    
    f->eax = retval; //return value
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
 }

void s_halt (void){
  //printf("halt\n");
  shutdown_power_off();
  NOT_REACHED();
}

void s_exit (void){
  thread_exit();
  NOT_REACHED();
}

// pid_t s_exec (const char *file);
// int s_wait (pid_t);
// bool s_create (const char *file, unsigned initial_size);
// bool s_remove (const char *file);
// int s_open (const char *file);
// int filesize (int fd);
// int read (int fd, void *buffer, unsigned length);

int s_write(int fd, const void *buffer, unsigned size){
  // printf("SYS_WRITE - fd: %d, buf: %s, size: %u\n",fd,buffer,(int)size);
  // printf("buff: %x\n",buffer);
  if (fd == 1)
    putbuf(buffer,size);
  return size; 
}
// void seek (int fd, unsigned position);
// unsigned tell (int fd);
// void close (int fd);

