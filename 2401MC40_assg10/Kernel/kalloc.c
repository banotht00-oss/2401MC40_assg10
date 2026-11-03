// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  int track_stats;
  struct run *freelist;
  uint allocated_pages;
  uint freed_pages;
  uint free_pages;
} kmem;

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  kmem.track_stats = 0;
  freerange(vstart, vend);
  // Count allocations made after the initial freelist has been populated.
  kmem.track_stats = 1;
}

void
kinit2(void *vstart, void *vend)
{
  // The second range is boot-time freelist initialization, not a runtime free.
  kmem.track_stats = 0;
  freerange(vstart, vend);
  kmem.use_lock = 1;
  kmem.track_stats = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
    kfree(p);
}
//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  kmem.free_pages++;
  if(kmem.track_stats)
    kmem.freed_pages++;
  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r){
    kmem.freelist = r->next;
    kmem.free_pages--;
    if(kmem.track_stats)
      kmem.allocated_pages++;
  }
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

// Return a consistent snapshot of allocator totals and the free-list size.
void
kallocstats(uint *allocated, uint *freed, uint *current, uint *freepages)
{
  if(kmem.use_lock)
    acquire(&kmem.lock);
  *allocated = kmem.allocated_pages;
  *freed = kmem.freed_pages;
  *current = kmem.allocated_pages - kmem.freed_pages;
  *freepages = kmem.free_pages;
  if(kmem.use_lock)
    release(&kmem.lock);
}
