#ifndef asdfasdf_fat
#define asdfasdf_fat

#include <stdint.h>

extern uint8_t f16_r_buffer[32];	// read buffer used by fat16 disk io
extern uint8_t f16_w_buffer[512];	// write buffer for fat16 

/*
 * Minimum saved data for fat 16 operation
 */
struct {
	uint32_t fat_start; // = 0x400C00;		// Addr of FAT
	uint32_t root_start; //=?				// Addr of root dir
	uint32_t data_start; // = 0x440000;		// Addr of data sector
	uint32_t sect_per_cluster; // = 128;	// Number of sectors (512 bytes) per cluster
	uint16_t file_start_cluster; //=5;		// Index of starting cluster of file
	uint16_t file_cur_cluster; // = 5;		// Index of file cluster being read
	uint16_t file_cur_pos;					// Current location in file (byte index)
	uint32_t file_size; // = 131072;		// Size of file being read, in bytes
	uint32_t global_cur_pos;				// Current location on disk
} f16_state;

/*
 * Sets read to specific addr on disk
 */
void f16_seek(uint32_t offset);

/*
 * Reads a block of data from disk to read/write buffer
 */
uint16_t f16_read(uint16_t count);


uint16_t f16_write(uint16_t count);


/* 
 * Reads partition table on disk.  Goes to first partition found.
 * Saves requried data from bootsector and partition table.
 * Sets read to start of root directory
 */
int f16_init();

void f16_seek_file(uint32_t position);

int f16_open_file(char *filename);

uint16_t f16_read_file(uint16_t bytes);

#endif
