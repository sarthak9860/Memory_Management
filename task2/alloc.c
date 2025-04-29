#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>

typedef struct block{
	int offset;
	int size;
	int free;
	struct block* next;
	struct block* prev;
}block;

void* mem_start = NULL;
block* block_list = NULL;

int init_alloc(){
	mem_start = mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (mmem_start == MAP_FAILED){
		return -1;
	}
	
}
