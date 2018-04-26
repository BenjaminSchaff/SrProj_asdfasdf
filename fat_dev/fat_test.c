/*!
 * @file fat_test.c
 *
 * @author Dan Paradis and Ben Schaff
 *
 * Test program for fat16 filesystem functions.
 * Reads a block of data from a file on a disk image containing a valid 
 * FAT16 filesystem.
 *
 */

#include <stdlib.h>
#include <stdio.h>

#include "f16_fs.h"

FILE *fin;

/*
 * Sets read to specific addr on disk
 */
void f16_disk_seek(uint32_t offset)
{
	fseek(fin, offset, SEEK_SET); //TODO make this use SD functions
}


/*
 * Reads a block of data from disk to read/write buffer
 */
uint16_t f16_disk_read(uint16_t count)
{
	// TODO, limit size to < 32 or buffer size?
	return (uint16_t)fread(f16_r_buffer, 1, count, fin); //TODO read from SD
	
	// for SD card, will require reading from two sectors, unless limited to only aligned reads.
}


uint16_t f16_disk_write(uint16_t count)
{
	// TODO do a read into write buffer, then write contents of buffer to disk
	// data move to proper offset in buffer
	// read before bytes
	// read after bytes
	// write block back to SD card
	return 0;
}


int main(int argc, char **argv) {
	char *filename;

	if (argc > 1) {
		fin = fopen(argv[1], "rb");
	} else {
		fin = fopen("test.img", "rb");
	}

		
	if (argc != 3) {
		filename = "LOG     TXT";
	} else {
		filename = argv[2];
	}

	// init filesysem
	f16_init(1);
	printf("\n");

	printf("Root ");
	f16_readdir();

	if (argc < 3) {
		fclose(fin);
		return 0;
	}
	
	f16_readdir();
	printf("\n");

	filename = argv[2];


	if(f16_open_file(filename)) {
		printf("Failed to open file\n");	
	} else {
		printf("Successfully opened file: %s\n", filename);
	}
	
	while(!f16_eof()) {
	
		if (!f16_read_file(sizeof(f16_r_buffer)))
			printf("Failire to read file\n");

	//	f16_r_buffer[sizeof(f16_r_buffer)-1] = 0;	
		printf("%.*s", sizeof(f16_r_buffer), f16_r_buffer);
	}
	printf("\n");
	fclose(fin);
	return 0;
}
