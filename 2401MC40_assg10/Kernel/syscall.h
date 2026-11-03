// System call numbers
#define SYS_fork    1
#define SYS_exit    2
#define SYS_wait    3
#define SYS_pipe    4
#define SYS_read    5
#define SYS_kill    6
#define SYS_exec    7
#define SYS_fstat   8
#define SYS_chdir   9
#define SYS_dup    10
#define SYS_getpid 11
#define SYS_sbrk   12
#define SYS_sleep  13
#define SYS_uptime 14
#define SYS_open   15
#define SYS_write  16
#define SYS_mknod  17
#define SYS_unlink 18
#define SYS_link   19
#define SYS_mkdir  20
#define SYS_close  21
#define SYS_vmprint 22       // Print the current process's page table.
#define SYS_getfaults 23     // Return this process's lazy page-fault count.
#define SYS_memstats 24      // Print physical page allocator statistics.
#define SYS_vtop 25          // Translate a mapped user virtual address.
#define SYS_pageinfo 26      // Print mapping and permission information.
#define SYS_setreadonly 27   // Clear write permission on a mapped user page.
