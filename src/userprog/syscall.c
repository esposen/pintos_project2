#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/malloc.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "threads/malloc.h"
#include "lib/string.h"
#include "lib/stdio.h"
#include "lib/debug.h"
#include "devices/shutdown.h"
#include "pagedir.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "userprog/process.h"

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
struct openfile *find_file(int);

void
syscall_init (void) 
{
  lock_init(&fslock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/*Eax is return value */
static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  verify_ptr((void *)f->esp,f); //Check if stack pointer is bad
  void *sp = f->esp;            //Copy stack pointer for safe manipulation
  // hex_dump(f->esp,f->esp,PHYS_BASE-f->esp,true); printf("\n");  
  
  int syscode = *(int *)sp; //Syscall code
  sp += sizeof(int *);
  
  //get_args put args in here
  uint32_t args[3];

  //various containers for return values 
  int retvali;
  bool retvalb;
  pid_t retvalp;
  unsigned retvalu;

  switch (syscode){
    /* Each case grabs relevant arguments from stack
    and calls associated sys call function */
    case SYS_HALT:
      s_halt();
      break;
      
    case SYS_EXIT: ;
      int status = *(int *)sp; //grab status and store in eax register
      s_exit(status,f);
      break;

    case SYS_EXEC:
      get_args(args,sp,1);
      args[0]= (uint32_t)usr_to_kernel((void *)args[0],f);
      retvalp = s_exec((char *)args[0]);
      f->eax = retvalp;
      break;
      
    case SYS_WAIT:
      get_args(args,sp,1);
      f->eax = s_wait(args[0]);
      break;

    case SYS_CREATE:
      get_args(args,sp,2);
      if((void *)args[0] == NULL) //filename can't be NULL
        s_exit(-1,f);
      args[0] = (uint32_t)usr_to_kernel((void *)args[0],f);
      lock_acquire(&fslock);
      retvalb = s_create((char *)args[0],(unsigned)args[1]);
      lock_release(&fslock);
      f->eax = retvalb;
      break;
      
    case SYS_REMOVE:
      get_args(args,sp,1);
      args[0] = (uint32_t)usr_to_kernel((void *)args[0],f);
      lock_acquire(&fslock);
      retvalb = s_remove((char *)args[0]);
      lock_release(&fslock);
      f->eax = retvalb;
      break;
      
    case SYS_OPEN:
      get_args(args,sp,1);
      args[0] = (uint32_t) usr_to_kernel((void *)args[0],f);
      lock_acquire(&fslock);
      retvali = s_open((char *)args[0]);
      lock_release(&fslock);
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


/* Halt system call. Halts Pintos by shutting down */
void s_halt (void){
  shutdown_power_off();
  NOT_REACHED();
}

/* Exit system call. Puts its status on the frame. */
void s_exit (int s, struct intr_frame *f){
  f->eax = s;
  struct thread *curr = thread_current();
  if(thread_exists(curr->parent))
    curr->child->status = s;
  printf("%s: exit(%d)\n",thread_current()->name,s);
  thread_exit();
  NOT_REACHED();
}

/* Exec system call. Executes command and returns pid of the new program  */
pid_t s_exec (const char *file){
  pid_t pid = process_execute(file);

  struct child_proc *c = get_child_proc(pid);
  //Make sure child exists
  ASSERT(c);
  //wait for child to load
  sema_down(&c->load_sema);
  if(c->load == LOAD_FAIL){
    return -1;
  }
  return pid;
}

/* Wait system call. Waits for child process to exit.  */
int s_wait (pid_t pid){
  return process_wait(pid);
}

/* Create system call. Creates a file, adding it to the filesystem */
bool s_create (const char *file, unsigned initial_size){
  //filename can't be empty
  if(strcmp(file,"") == 0) return false;
  
  bool success = filesys_create(file, initial_size);
  return success;
}

/* Remove system call. Deletes a file from the filesystem */
bool s_remove (const char *file){
  //filename can't be empty
  if(strcmp(file,"") == 0) return false;
  
  bool success = filesys_remove(file);
  return success;
}

/* Open system call. Opens a file, giving it a process and file unique file descriptor. 
   Returns file descriptor */
int s_open (const char *file){
  struct openfile *of = malloc(sizeof(struct openfile *));
  struct thread *t = thread_current();
  
  //Filename can't be NULL or empty
  if(file == NULL) return -1; 
  if(strcmp(file,"") == 0) return -1;
  
  struct file *f = filesys_open(file); //open file
  if(f == NULL) return -1; //if NULL ptr, failed to open

  //fill openfile struct and push onto list
  of->fd = t->lastfd++;
  of->fileptr = f;
  list_push_front(&t->openfiles,&of->fileelem);

  return of->fd;
}

/*Close system call. Closes a file descriptor. Done implicility on exiting process. */
void s_close (int fd){
  struct list_elem *e;
  struct thread *t = thread_current();
  if(list_empty(&t->openfiles)) return;
  
  for(e = list_begin(&t->openfiles);
      e != list_end(&t->openfiles);
      e = list_next(e)){
    struct openfile *of = list_entry(e, struct openfile,fileelem);
    if(of->fd == fd){
      list_remove(e);
      free(of);
      break;
    }
  }
}

/*Read system call. Reads length characters from file, storing it in buffer. */
int s_read (int fd, void *buffer, unsigned length){
   off_t size = 0;
   
   //TODO: Make FD==0 READ FROM KEYBOARD USING input_getc();
   if(fd == 0) return 0;

   struct openfile *of = find_file(fd);
   if(of == NULL) return 0;
   
   size = file_read(of->fileptr,buffer,length);
   return (int)size;
 }
 
/*Write system call. Writes size characters from buffer to file. */
int s_write(int fd, const void *buffer, unsigned size){
  off_t bytes_written = 0;

  if (fd == 1) //fd == 1 is console 
    putbuf(buffer,size);

  struct openfile *of = find_file(fd);
  if(of == NULL) return 0;

  bytes_written = file_write(of->fileptr,buffer,size);
  return (int) bytes_written;
}

//void seek (int fd, unsigned position)
// unsigned tell (int fd);

/* Filesize system call. Returns filesize of file associated with file descriptor.
  Size in bytes*/
int s_filesize (int fd){
  off_t size = 0;
  
  struct openfile *of = find_file(fd);
  if(of == NULL) return 0;
  
  size = file_length(of->fileptr);
  return (int) size; 
}
 
/*Get arguments off the stack and places into buff*/
void get_args(uint32_t *buf, void *esp,int argc){
  void *sp = esp;
  
  for(int i=0;i<argc;i++){
    buf[i] = *(int *)sp;
     sp += sizeof(int *);    
  }
}

/* Verifies within valid user virtual memory  */
void verify_ptr(void *ptr,struct intr_frame *f){
  if(!is_user_vaddr(ptr) || ptr<UADDR_BOTTOM) s_exit(-1,f);
}

/* Converts to physical memory */
void *usr_to_kernel(void *vaddr, struct intr_frame *f){
  verify_ptr(vaddr,f);
  void *p = pagedir_get_page(thread_current()->pagedir,vaddr);
  if(p == NULL) s_exit(-1,f);
  
  return p;
}

/* Given file descripter FD, finds associated file struct 
  NOTE: Not safe for multi-threading */
struct openfile *find_file(int fd){
  struct thread *t = thread_current();
  struct list_elem *e;
  for(e = list_begin(&t->openfiles);
      e != list_end(&t->openfiles);
      e = list_next(e)){
    struct openfile *of = list_entry(e, struct openfile,fileelem);
    if(of->fd == fd) return of;
  }
  return NULL;
}