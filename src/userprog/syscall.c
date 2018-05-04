#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/malloc.h"
#include "threads/vaddr.h"
#include "lib/string.h"
#include "lib/stdio.h"
#include "lib/debug.h"
#include "devices/shutdown.h"
#include "pagedir.h"
#include "filesys/filesys.h"
#include "filesys/file.h"

#define UADDR_BOTTOM ((void *) 0x08048000)

static void syscall_handler (struct intr_frame *);

void s_halt (void) NO_RETURN;
void s_exit (int, struct intr_frame *) NO_RETURN;
pid_t s_exec (const char *);
int s_wait (pid_t);
bool s_create (const char *, unsigned);
bool s_remove (const char *);
int s_open (const char *);
int s_filesize (int);
int s_read (int, void *, unsigned);
int s_write(int , const void *, unsigned);
void s_seek (int, unsigned);
unsigned s_tell (int);
void s_close (int);

//Helper Function
void get_args(uint32_t *buf, void *esp,int argc);
void verify_ptr(void *,struct intr_frame *);
void *usr_to_kernel(void *, struct intr_frame *);

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
  case SYS_HALT:
    s_halt();
    break;
    
  case SYS_EXIT: ;
    int status = *(int *)sp; //grab status and store in eax register
    s_exit(status,f);
    break;

    //Exec + Wait
  case SYS_EXEC:
    break;
    
  case SYS_WAIT:
    break;

    //File manip
  case SYS_CREATE:
    get_args(args,sp,2);
    if((void *)args[0] == NULL) //filename can't be NULL
      s_exit(-1,f);
    args[0] = (uint32_t)usr_to_kernel((void *)args[0],f);
    retvalb = s_create((char *)args[0],(unsigned)args[1]);
    f->eax = retvalb;
    break;
    
  case SYS_REMOVE:
    get_args(args,sp,1);
    args[0] = (uint32_t)usr_to_kernel((void *)args[0],f);
    retvalb = s_remove((char *)args[0]);
    f->eax = retvalb;
    break;
    
  case SYS_OPEN:
    get_args(args,sp,1);
    args[0] = (uint32_t) usr_to_kernel((void *)args[0],f);
    retvali = s_open((char *)args[0]);
    f->eax = retvali;
    break;
    
  case SYS_CLOSE:
    get_args(args,sp,1);
    if(args[0]== 0 || args[0] == 1) s_exit(-1,f);
    s_close(args[0]);
    break;
    
  case SYS_FILESIZE:
    get_args(args,sp,1);
    retvali = s_filesize(args[0]);
    f->eax = retvali; 
    break;

    //Read+Write
  case SYS_READ:
    get_args(args,sp,3);
    args[1] = (uint32_t) usr_to_kernel((void *)args[1],f);
    
    retvali = s_read((int)args[0],(void *)args[1],(unsigned)args[2]);
    f->eax = retvali;
    break;
    
  case SYS_WRITE:
    get_args(args,sp,3);
    args[1] = (uint32_t) usr_to_kernel((void *)args[1],f);
    
    retvali = s_write((int)args[0],(void *)args[1],(unsigned)args[2]);
    f->eax = retvali;
    break;
    
  case SYS_SEEK :
    break;
    
  case SYS_TELL:
    break;

  default:
    printf("syscode not found");
    break;
  }
 }

void s_halt (void){
  shutdown_power_off();
  NOT_REACHED();
}

void s_exit (int s, struct intr_frame *f){
  f->eax = s;
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

bool s_remove (const char *file){
  //filename can't be empty
  if(strcmp(file,"") == 0) return false;
  
  bool success = filesys_remove(file);
  return success;
}

int s_open (const char *file){
  struct openfile of;
  struct thread *t = thread_current();

  //Filename can't be NULL or empty
  if(file == NULL) return -1; 
  if(strcmp(file,"") == 0) return -1;
  
  struct file *f = filesys_open(file); //open file
  if(f == NULL) return -1; //if NULL ptr, failed to open

  //fill openfile struct and push onto list
  of.fd = t->lastfd++;
  of.fileptr = f;
  list_push_front(&t->openfiles,&of.fileelem);
  
  return of.fd;
}

void s_close (int fd){
  struct list_elem *e;
  struct list l = thread_current()->openfiles;

  // printf("wanted fd: %d\n",fd);
  if(list_empty(&thread_current()->openfiles)) return;

  for(e = list_begin(&l); e != list_end(&l); e = list_next(e)){
    struct openfile *of = list_entry(e, struct openfile,fileelem);
    if(of->fd == fd){
      list_remove(e);
      break;
    }
  }
}

int s_filesize (int fd){
  struct list_elem *e;
  struct list l = thread_current()->openfiles;

  if(list_empty(&thread_current()->openfiles)) return -1;

  for(e = list_begin(&l); e != list_end(&l); e = list_next(e)){
    struct openfile *of = list_entry(e, struct openfile,fileelem);
    if(of->fd == fd){
      off_t size = file_length(of->fileptr);
      return (int)size;
    }
  }
  return -1;
}

int s_read (int fd, void *buffer, unsigned length){
  struct list_elem *e;
  struct list l = thread_current()->openfiles;

  if(list_empty(&thread_current()->openfiles)) return -1;

  //TODO: Make FD==0 READ FROM KEYBOARD USING input_getc();
  if(fd == 0) return -1;

  for(e = list_begin(&l); e != list_end(&l); e = list_next(e)){
    struct openfile *of = list_entry(e, struct openfile,fileelem);
    if(of->fd == fd){
      off_t size = file_read(of->fileptr,buffer,length);
      return (int)size;
    }
  }
  return -1;
  
}

int s_write(int fd, const void *buffer, unsigned size){
  // printf("SYS_WRITE - fd: %d, buf: %s, size: %u\n",fd,buffer,(int)size);
  // printf("buff: %x\n",buffer);
  if (fd == 1)
    putbuf(buffer,size);
  return size; 
}
// void seek (int fd, unsigned position);
// unsigned tell (int fd);

void get_args(uint32_t *buf, void *esp,int argc){
  void *sp = esp;

  for(int i=0;i<argc;i++){
    buf[i] = *(int *)sp;
    sp += sizeof(int *);    
  }
}

/* verifies within valid user virtual memory  */
void verify_ptr(void *ptr,struct intr_frame *f){
  if(!is_user_vaddr(ptr) || ptr<UADDR_BOTTOM) s_exit(-1,f);
}

/* converts to physical memory */
void *usr_to_kernel(void *vaddr, struct intr_frame *f){
  verify_ptr(vaddr,f);
  void *p = pagedir_get_page(thread_current()->pagedir,vaddr);
  if(p == NULL) s_exit(-1,f);
  
  return p;
} 
