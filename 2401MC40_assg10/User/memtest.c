#include "types.h"
#include "user.h"

#define PGSIZE 4096
#define NPAGES 3

int
main(void)
{
  int i;
  char *mem;

  mem = sbrk(NPAGES * PGSIZE);
  if(mem == (char*)-1){
    printf(2, "memtest: sbrk failed\n");
    exit();
  }

  // Touch each reserved page so lazy allocation obtains physical frames.
  for(i = 0; i < NPAGES; i++)
    mem[i * PGSIZE] = 1;

  printf(1, "After allocating and touching %d pages:\n", NPAGES);
  memstats();
  if(sbrk(-NPAGES * PGSIZE) == (char*)-1)
    printf(2, "memtest: failed to release memory\n");
  printf(1, "After releasing the allocation:\n");
  memstats();
  exit();
}
