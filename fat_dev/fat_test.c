#include <stdlib.h>
#include <stdio.h>

#include "f16_fs.h"

FILE *fin;

/*
 * Sets read to specific addr on disk
 */
void f16_seek(uint32_t offset)
{
	fseek(fin, offset, SEEK_SET); //TODO make this use SD functions
}


/*
 * Reads a block of data from disk to read/write buffer
 */
uint16_t f16_read(uint16_t count)
{
	// TODO, limit size to < 32 or buffer size?
	return (uint16_t)fread(f16_r_buffer, 1, count, fin); //TODO read from SD
	
	// for SD card, will require reading from two sectors, unless limited to only aligned reads.
}


uint16_t f16_write(uint16_t count)
{
	// TODO do a read into write buffer, then write contents of buffer to disk
	


	// data move to proper offset in buffer

	// read before bytes

	// read after bytes

	// write block back to SD card
	
	

	return 0;
}


int main(int argc, char **argv) {

	if (argc > 1) {
		fin = fopen(argv[1], "rb");
	} else {
		fin = fopen("test.img", "rb");
	}
	
	char filename[12] = "LOG     TXT";
	
	// init filesysem
	f16_init();

	if(f16_open_file(filename)) {
		printf("Failed to open file\n");	
	} else {
		printf("Successfully openned file: %s", filename);
	}


	f16_read_file(32);


	f16_r_buffer[sizeof(f16_r_buffer)-1] = 0;	
	printf("%s\n",f16_r_buffer);


	fclose(fin);
	return 0;
}

