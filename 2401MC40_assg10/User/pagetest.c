#include "types.h"
#include "user.h"

#define PGSIZE 4096

int
main(void)
{
  char *mem, *page;
  int pid;

  mem = sbrk(2 * PGSIZE);
  if(mem == (char*)-1){
    printf(2, "pagetest: sbrk failed\n");
    exit();
  }
  page = (char*)(((uint)mem + PGSIZE - 1) & ~(PGSIZE - 1));
  page[0] = 'A';

  printf(1, "Before protection:\n");
  if(pageinfo((int)page) < 0){
    printf(2, "pagetest: page is not mapped\n");
    exit();
  }
  if(setreadonly((int)page) < 0){
    printf(2, "pagetest: setreadonly failed\n");
    exit();
  }
  printf(1, "After removing write permission:\n");
  pageinfo((int)page);

  // Isolate the intentional protection fault so the parent can report it.
  pid = fork();
  if(pid < 0){
    printf(2, "pagetest: fork failed\n");
    exit();
  }
  if(pid == 0){
    printf(1, "Child: attempting a write to the read-only page\n");
    page[0] = 'B';
    printf(1, "pagetest: unexpected write success\n");
    exit();
  }
  wait();
  printf(1, "Parent: child exited after the expected protection fault\n");
  exit();
}
