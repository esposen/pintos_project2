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
#include "filesys/filesys.h"

static void syscall_handler (struct intr_frame *);

void s_halt (void) NO_RETURN;
void s_exit (int) NO_RETURN;
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
void get_args(uint32_t *buf, void *esp,int argc);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/*Eax is return value */
static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  void *sp = f->esp; 
  // hex_dump(f->esp,f->esp,PHYS_BASE-f->esp,true); printf("\n");  
  int syscode = *(int *)sp;
  sp += sizeof(int *);
  //printf("System Call: %d\n",syscode);

  //get_args put args in here
  uint32_t args[3];

  //various containers for return values 
  int retvali;
  bool retvalb;
  pid_t retvalp;
  unsigned retvalu;

  switch (syscode){
    //Halt + Exit
  case SYS_HALT: ;
    s_halt();
    break;
  case SYS_EXIT: ;
    int status = *(int *)sp; //grab status and store in eax register
    f->eax = status;
    s_exit(status);
    break;

    //Exec + Wait
  case SYS_EXEC: ;
    break;
  case SYS_WAIT: ;
    break;

    //File manip
  case SYS_CREATE: ;
    get_args(args,sp,2);
    if(args[0] == NULL){ //filename can't be NULL
      f->eax = -1;
      s_exit(-1);
    }
    retvalb = s_create((char *)args[0],(unsigned)args[1]);
    f->eax = retvalb;
    break;
  case SYS_REMOVE: ;
    break;
    
  case SYS_OPEN: ;
    get_args(args,sp,1);
    retvali = s_open((char *)args[0]);
    f->eax = retvali;
    break;
  case SYS_FILESIZE: ;
    break;

    //Read+Write
  case SYS_READ: ;
    break;  
  case SYS_WRITE: ;
    get_args(args,sp,3);
    retvali = s_write((int)args[0],(void *)args[1],(unsigned)args[2]);
    f->eax = retvali;
    break;
  case SYS_SEEK : ;
    break;
  case SYS_TELL: ;
    break;

    //Why is close down here???
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

void s_exit (int s){
  printf("%s: exit(%d)\n",thread_current()->name,s);
  thread_exit();
  NOT_REACHED();
}

// pid_t s_exec (const char *file);
// int s_wait (pid_t);

bool s_create (const char *file, unsigned initial_size){
  //filename can't be empty
  if(strcmp(file,"") == 0) return false;
  
  bool success = filesys_create(file, initial_size);
  return success;
}

// bool s_remove (const char *file);
int s_open (const char *file){
  struct openfile of;
  struct thread *t = thread_current(); //list stored in thread struct

  //Filename can't be NULL or empty
  if(file == NULL) return -1; 
  if(strcmp(file,"") == 0) return -1;
  
  struct file *f = filesys_open(file); //open file
  if(f == NULL) return -1; //if NULL ptr, failed to open

  //fill openfile struct and push onto list
  of.fd = t->lastfd++;
  of.fileptr = f;
  list_push_back(&t->openfiles,&of.fileelem);

  return of.fd;
}
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

void get_args(uint32_t *buf, void *esp,int argc){
  void *sp = esp;

  for(int i=0;i<argc;i++){
    buf[i] = *(int *)sp;
    sp += sizeof(int *);    
  }
}
