#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;
  uint newsz;
  struct proc *p = myproc();

  if(argint(0, &n) < 0)
    return -1;
  addr = p->sz;
  if(n > 0){
    // Reserve virtual address space now; physical pages are allocated on faults.
    newsz = p->sz + n;
    if(newsz < p->sz || newsz >= KERNBASE)
      return -1;
    p->sz = newsz;
  } else if(n < 0){
    if((uint)(-(n + 1)) + 1 > p->sz || growproc(n) < 0)
      return -1;
  }
  return addr;
}

int
sys_vmprint(void)
{
  // Print the page table belonging to the calling process.
  vmprint(myproc()->pgdir);
  return 0;
}

int
sys_getfaults(void)
{
  // Report only faults handled for the calling process.
  return myproc()->numfaults;
}

// Display a consistent snapshot of physical page allocator counters.
int
sys_memstats(void)
{
  uint allocated, freed, current, freepages;

  kallocstats(&allocated, &freed, &current, &freepages);
  cprintf("Physical pages: allocated=%d freed=%d current=%d free=%d\n",
          allocated, freed, current, freepages);
  return 0;
}

// Translate only addresses in the caller's user address space.
int
sys_vtop(void)
{
  int address;
  struct proc *p;

  if(argint(0, &address) < 0)
    return -1;
  p = myproc();
  if((uint)address >= p->sz || (uint)address >= KERNBASE)
    return -1;
  return vm_vtop(p->pgdir, (uint)address);
}

// Report page-table information for a mapped address in the caller.
int
sys_pageinfo(void)
{
  int address;
  struct proc *p;

  if(argint(0, &address) < 0)
    return -1;
  p = myproc();
  if((uint)address >= p->sz || (uint)address >= KERNBASE){
    cprintf("pageinfo: virtual address 0x%x is outside the process\n",
            (uint)address);
    return -1;
  }
  return vm_pageinfo(p->pgdir, (uint)address);
}

// Protect one mapped user page against writes.
int
sys_setreadonly(void)
{
  int address;
  struct proc *p;

  if(argint(0, &address) < 0)
    return -1;
  p = myproc();
  if((uint)address >= p->sz || (uint)address >= KERNBASE)
    return -1;
  return vm_setreadonly(p->pgdir, (uint)address);
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
