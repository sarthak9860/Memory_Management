// Question

/** REQUIRED HEADER FILES **/

#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include"../include/head.h"

/*
 * Name of the function:
 * Author: Sarthak Bosamiya
 * Created: 28-04-2025
 * Modified: 28-04-2025
*/

void task3(){
        printf("Process ID: %d\n", getpid());
        void* addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (addr == MAP_FAILED){
                perror("Map Failed");
                return;
        }
        printf("Mapped one page at address: %p\n", addr);

	((char*)addr)[0] = 'S';

	printf("Wrote '%s' to the memory page\n", (char*)addr);
        printf("Program is running......... Press Enter key to exit\n");
        //sleep(200);
        getchar();
        getchar();
        munmap(addr, 4096);
        printf("Program Finished\n");
}
