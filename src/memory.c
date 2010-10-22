#include "devel/memory.h"
#include "memmap.h"

void mem_init(uint32_t totalmemory)
{
	system_memory_size = totalmemory;

	debug_write_string("mem_init(");
	debug_write_uint32(system_memory_size);
	debug_write_string(")\n");

	uint32_t c;
	uint32_t blocks = totalmemory / (uint32_t)MEMORY_BLOCK_SIZE / 8; // memory bitmap
	uint32_t memory_bitmap_size = ( (blocks /8) / (uint32_t)MEMORY_BLOCK_SIZE ) +1;	// always waste 1 block

	debug_write_string("blocks in memory map: ");
	debug_write_uint32(blocks);
	debug_write_string("\n");

	debug_write_string("memory map size: ");
	debug_write_uint32(memory_bitmap_size);
	debug_write_string("\n");

	uint8_t *ptrmem = system_memory_map;
	uint32_t size_of_memory_map = blocks/8;

	// mark all memory map as free
	for (c=0;c<blocks;c++)
	{
		*ptrmem = 0x00;
		ptrmem++;
	}
	ptrmem = system_memory_map;
	// always mark the first megabyte as used
	for (c=0;c<((1024*1024)/(uint32_t)MEMORY_BLOCK_SIZE / 8);c++)
	{
		*ptrmem = 0xff;
		ptrmem++;
	}
	// mark the blocks of memory map size as used
	for (c=0;c<memory_bitmap_size;c++)
	{
		*ptrmem = 0xff;
		ptrmem++;
	}

	// show memory map to debugger
	ptrmem = system_memory_map;
	debug_write_string("Memory bitmap:\n");
	for (c=0;c<blocks;c++)
	{
		debug_write_uint8(*ptrmem);
		debug_write_string(" ");
		ptrmem++;
	}
	debug_write_string("\n");

	debug_write_string("mem_init() return\n");
}

uint32_t * malloc(uint32_t size)
{
	debug_write_string("malloc(");
	debug_write_uint32(size);
	debug_write_string(")\n");

	uint32_t blocks = system_memory_size / (uint32_t)MEMORY_BLOCK_SIZE / 8; // memory bitmap
	uint32_t blocks_to_search = (size / (uint32_t)MEMORY_BLOCK_SIZE / 8 )+1; // always waste a block
	uint32_t c;
	uint8_t *ptrmem = system_memory_map;
	uint32_t malloc_return = 0x0;

	debug_write_string("Searching hole of size ");
	debug_write_uint32(blocks_to_search);
	debug_write_string(" in memory bitmap:\n");
	for (c=0;c<blocks;c++)
	{
		// search for hole of blocks_to_search
		debug_write_uint8(*ptrmem);
		debug_write_string(" ");
		ptrmem++;
	}
	debug_write_string("\n");
	debug_write_string("malloc() return: ");
	debug_write_uint32(malloc_return);
	debug_write_string("\n");
	return malloc_return;
}
