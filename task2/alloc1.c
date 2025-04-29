#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "alloc.h"

#define META_BLOCKS 512  // max 512 blocks metadata (enough for 4KB / 8 = 512)

typedef struct block {
	size_t offset;        // Offset into mem_start
	size_t size;          // Size of the block
	int free;             // 1 = free, 0 = allocated
	int next;             // Index of next block (-1 if none)
	int prev;             // Index of previous block (-1 if none)
} block_t;

static void *mem_start = NULL;     // Start of 4KB user page
static block_t *meta_start = NULL; // Start of 4KB metadata page
static int head = -1;              // Head index in meta_start

// Align to 8 bytes
static size_t align8(size_t size) {
	return (size + 7) & ~7;
}

int init_alloc() {
	mem_start = mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (mem_start == MAP_FAILED) {
		return -1;
	}

	meta_start = mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (meta_start == MAP_FAILED) {
		munmap(mem_start, PAGESIZE);
		return -1;
	}

	// Initialize metadata
	block_t *blocks = (block_t *)meta_start;
	blocks[0].offset = 0;
	blocks[0].size = PAGESIZE;
	blocks[0].free = 1;
	blocks[0].next = -1;
	blocks[0].prev = -1;
	head = 0;

	return 0;
}

int cleanup() {
	if (munmap(mem_start, PAGESIZE) == -1) {
		return -1;
	}
	if (munmap(meta_start, PAGESIZE) == -1) {
		return -1;
	}
	mem_start = NULL;
	meta_start = NULL;
	head = -1;
	return 0;
}

// Find a free block and optionally split it
char *alloc(int size) {
	if (size <= 0 || size % 8 != 0) {
		return NULL;
	}

	size = align8(size);
	block_t *blocks = (block_t *)meta_start;
	int curr = head;

	while (curr != -1) {
		if (blocks[curr].free && blocks[curr].size >= size) {
			// Split if necessary
			if (blocks[curr].size > size) {
				// Find free block slot
				int new_block = -1;
				for (int i = 1; i < META_BLOCKS; i++) {
					if (blocks[i].size == 0 && blocks[i].offset == 0) {
						new_block = i;
						break;
					}
				}
				if (new_block == -1) {
					return NULL; // No metadata slot available
				}

				blocks[new_block].offset = blocks[curr].offset + size;
				blocks[new_block].size = blocks[curr].size - size;
				blocks[new_block].free = 1;
				blocks[new_block].next = blocks[curr].next;
				blocks[new_block].prev = curr;

				if (blocks[curr].next != -1) {
					blocks[blocks[curr].next].prev = new_block;
				}

				blocks[curr].next = new_block;
				blocks[curr].size = size;
			}

			blocks[curr].free = 0;
			return (char *)mem_start + blocks[curr].offset;
		}
		curr = blocks[curr].next;
	}
	return NULL; // No suitable block found
}

void dealloc(char *ptr) {
	if (!ptr || !mem_start) {
		return;
	}

	size_t offset = (size_t)(ptr - (char *)mem_start);
	block_t *blocks = (block_t *)meta_start;
	int curr = head;

	while (curr != -1) {
		if (blocks[curr].offset == offset) {
			blocks[curr].free = 1;

			// Merge with next
			if (blocks[curr].next != -1 && blocks[blocks[curr].next].free) {
				int next = blocks[curr].next;
				blocks[curr].size += blocks[next].size;
				blocks[curr].next = blocks[next].next;
				if (blocks[next].next != -1)
					blocks[blocks[next].next].prev = curr;

				// Clear next block
				blocks[next].offset = 0;
				blocks[next].size = 0;
				blocks[next].free = 0;
				blocks[next].next = -1;
				blocks[next].prev = -1;
			}

			// Merge with prev
			if (blocks[curr].prev != -1 && blocks[blocks[curr].prev].free) {
				int prev = blocks[curr].prev;
				blocks[prev].size += blocks[curr].size;
				blocks[prev].next = blocks[curr].next;
				if (blocks[curr].next != -1)
					blocks[blocks[curr].next].prev = prev;

				// Clear current block
				blocks[curr].offset = 0;
				blocks[curr].size = 0;
				blocks[curr].free = 0;
				blocks[curr].next = -1;
				blocks[curr].prev = -1;
			}

			return;
		}
		curr = blocks[curr].next;
	}
}

