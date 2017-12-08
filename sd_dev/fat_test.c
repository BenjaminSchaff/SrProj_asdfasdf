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
	// TODO, limit size to <512?
	return (uint16_t)fread(f16_r_buffer, 1, count, fin); //TODO read from SD
}


uint16_t f16_write(uint16_t count)
{
	// TODO write contents of buffer to disk
	return 0;
}


int main(int argc, char **argv) {

	if (argc > 1) {
		fin = fopen(argv[1], "rb");
	} else {
		fin = fopen("test.img", "rb");
	}
	

	f16_init();

	f16_read_file(32);


	f16_r_buffer[sizeof(f16_r_buffer)-1] = 0;	
	printf("%s\n",f16_r_buffer);


	fclose(fin);
	return 0;
}

