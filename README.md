xv6 Memory Management and Page Replacement Assignment
This repository contains the implementation of the xv6 operating system memory management and page replacement assignment.

Features Implemented
Physical Memory Tracking (kalloc.c & memtest.c): Added counters and spinlocks to track total allocated and freed physical pages, exposed via the memstats system call.

Virtual-to-Physical Address Translation (vm.c & vttest.c): Implemented the vtop system call to walk page tables and translate virtual addresses to physical frame addresses.

Page Table Management & Protection (pagetest.c): Implemented pageinfo to inspect physical frames and permissions (PTE_P, PTE_W, PTE_U), and setreadonly to clear write permissions and test page fault handling.

Page Replacement Algorithms (pagereplace.c): Developed a simulation program for FIFO, LRU, and Optimal page replacement algorithms across varying frame sizes.

How to Build and Run
Open your terminal inside the xv6 source directory (running on Linux/WSL).

Compile and boot xv6 in QEMU:

Bash
make clean
make qemu
Inside the xv6 shell, run the test programs:

Bash
$ memtest
$ vttest
$ pagetest
$ pagereplace
