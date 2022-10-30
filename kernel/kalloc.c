// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);
void kfreeinit(void *pa);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct kmem{
  struct spinlock lock;
  struct run *freelist;
};

struct kmem kmems[NCPU];

void
kinit()
{
  //给每个CPU初始化一个lock
  for(int i=0; i<NCPU; i++){
      initlock(&kmems[i].lock, "kmem");
  }
  freerange(end, (void*)PHYSTOP);

}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfreeinit(p); //重写kfree，为每个CPU新建独立的freelist
}

void
kfreeinit(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;
  int cpu_num,cpu_size;
  //将空间均分为NCPU份
  cpu_size = ((void*)PHYSTOP - (void*)end)/NCPU;
  //根据取整结果确定cpu号，然后分配freelist
  cpu_num = (pa - (void*)end)/cpu_size;

  acquire(&kmems[cpu_num].lock);
  r->next = kmems[cpu_num].freelist;
  kmems[cpu_num].freelist = r;
  // printf("%d",cpu_num);
  release(&kmems[cpu_num].lock);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;
  int cpu_num;
  push_off();
  cpu_num = cpuid();
  pop_off();

  acquire(&kmems[cpu_num].lock);
  r->next = kmems[cpu_num].freelist;
  kmems[cpu_num].freelist = r;
  release(&kmems[cpu_num].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;
  int cpu_num;
  push_off();
  cpu_num = cpuid();
  pop_off();


  acquire(&kmems[cpu_num].lock);
  r = kmems[cpu_num].freelist;
  if(r)
    kmems[cpu_num].freelist = r->next;
  release(&kmems[cpu_num].lock);
  if(!r){
    //如果本cpu的freelist没有，则遍历其他cpu，从其他CPU的freelist中窃取内存块
    for(int i = 0 ; i < NCPU ; i++)
    {
      if(i == cpu_num) 
        continue;   //跳过自己
      acquire(&kmems[i].lock);//上锁
      r = kmems[i].freelist;
      if(r)
      {
        kmems[i].freelist = r->next;
        release(&kmems[i].lock);//解锁
        break;  //找到直接跳出循环
      }
      release(&kmems[i].lock);//解锁
    }
  }


  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
