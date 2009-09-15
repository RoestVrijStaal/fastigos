#include <stdio.h>
#define MEMORY_MAP_BYTES 63

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;

uint8_t mem_register[MEMORY_MAP_BYTES];
//uint32_t mem_list[MEMORY_MAP_BYTES * 8];

void print_mem_bitmap(void)
{
	uint8_t block;
	uint8_t pos_byte;
	uint8_t pos_bit;

	for (pos_byte=0; pos_byte <= MEMORY_MAP_BYTES; pos_byte++)
	{
		for ( pos_bit=0; pos_bit <= 7; pos_bit++)
		{
			block = mem_register[pos_byte] >> pos_bit & 0x01;
			printf("%i", block);
		}
		printf(", ");
	}
	printf("\n");
}

void set_blocks(uint16_t numblocks, uint16_t byte, uint8_t bit)
{
	uint16_t i;
	uint8_t block;
	uint8_t mask;

	for ( i = 1; i<=numblocks; i++)
	{
		mask = 0x1;
		mask = mask << bit;
		mem_register[byte] = mem_register[byte] ^ mask;
		bit++;
		if ( bit > 7 )
		{
			byte++;
			bit = 0;
		}
	}
}

uint16_t block_malloc(uint16_t numblocks)
{
	uint8_t block;
	uint8_t pos_byte;
	uint8_t pos_bit;
	uint16_t countadjacentblocks = 1;
	uint8_t saved_pos_byte;
	uint8_t saved_pos_bit;

	for (pos_byte=0; pos_byte <= MEMORY_MAP_BYTES; pos_byte++)
	{
		for ( pos_bit=0; pos_bit <= 7; pos_bit++)
		{
			block = mem_register[pos_byte] >> pos_bit & 0x01;
			if ( block == 0 )
			{
				if ( countadjacentblocks == 1 )
				{
					// save the position
					saved_pos_byte = pos_byte;
					saved_pos_bit = pos_bit;
				}
				countadjacentblocks++;
				if ( countadjacentblocks > numblocks ) 
				{
					// bigger hole to allocate the data!
					//printf("byte: %i, bit: %i\n", saved_pos_byte, saved_pos_bit);
					set_blocks(numblocks, saved_pos_byte, saved_pos_bit);
					return ( saved_pos_byte * 8 ) + saved_pos_bit;
				}
			}
			else
			{
				countadjacentblocks = 1;
			}
		}
	}
	return 0;
}

void mm_init(void)
{
	uint8_t i;

	for (i=0;i<=MEMORY_MAP_BYTES;i++)
	{
		mem_register[i] = 0x00;
	}
	mem_register[0]=0xff;		// reserve first megabyte + stack
	mem_register[1]=0xff;		// reserve first megabyte + stack
	mem_register[2]=0x01;		// reserve first megabyte + stack
}
uint32_t resb(uint32_t size)
{
	uint32_t i;
	uint16_t blocks;

	blocks = ( size / 65536 ) +1;
	i = block_malloc(blocks);
	i = i * 65536;
	return i;
}
uint8_t freeb(uint32_t base, uint32_t size)
{
	uint8_t pos_byte;
	uint8_t pos_bit;
	uint16_t bsize;

	bsize = (size / 65536) +1;
	pos_byte = (base / 65536) / 8;
	pos_bit = (base / 65535 ) % 8;
	set_blocks(bsize, pos_byte, pos_bit);
}

int main(int argc, char *argv[])
{
	uint32_t i, i2;

	mm_init();
	i = resb(65536);
	print_mem_bitmap();
	printf("\n");
	freeb(i, 65536);
	print_mem_bitmap();

	return 0;
}
