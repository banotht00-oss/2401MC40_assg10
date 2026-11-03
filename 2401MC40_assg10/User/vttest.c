#include "types.h"
#include "user.h"

#define PGSIZE 4096

int
main(void)
{
  char *mem, *base;
  int pa;

  mem = sbrk(4 * PGSIZE);
  if(mem == (char*)-1){
    printf(2, "vttest: sbrk failed\n");
    exit();
  }
  base = (char*)(((uint)mem + PGSIZE - 1) & ~(PGSIZE - 1));

  // Materialize two pages; leave the middle page reserved but unmapped.
  base[0] = 1;
  base[2 * PGSIZE] = 2;
  pa = vtop((int)(base + 37));
  printf(1, "mapped address 0x%x -> physical 0x%x\n",
         (uint)(base + 37), pa);
  pa = vtop((int)(base + 2 * PGSIZE + 123));
  printf(1, "second mapped address 0x%x -> physical 0x%x\n",
         (uint)(base + 2 * PGSIZE + 123), pa);
  pa = vtop((int)(base + PGSIZE));
  printf(1, "reserved but untouched address 0x%x -> %d (expected -1)\n",
         (uint)(base + PGSIZE), pa);
  pa = vtop(0x7ffff000);
  printf(1, "outside-process address 0x7ffff000 -> %d (expected -1)\n", pa);

  if(sbrk(-4 * PGSIZE) == (char*)-1)
    printf(2, "vttest: failed to release memory\n");
  exit();
}
