/*!
 * @file f16_ds.h
 *
 * @author Dan Paradis and Ben Schaff
 *
 * Functions for implimenting a FAT16 filesystem.
 *
 * Functional to a limited capacity.  Does not support files larger than
 * one cluster (64kB), files in subdirectories, or creating new files,
 * or modifying the size of files.
 *
 */

#ifndef asdfasdf_fat
#define asdfasdf_fat

#include <stdint.h>

extern uint8_t f16_r_buffer[32];	// read buffer used by fat16 disk io
extern uint8_t f16_w_buffer[512];	// write buffer for fat16 

/*!
 * Data struction to hold minimum saved info for fat 16 operation
 */
struct {
	uint32_t fat_start; 		// Addr of FAT
	uint32_t root_start;		// Addr of root dir
	uint32_t data_start;		// Addr of data sector
	uint32_t sect_per_cluster;	// Number of sectors (512 bytes) per cluster
	uint16_t file_start_cluster;// Index of starting cluster of file
	uint16_t file_cur_cluster;	// Index of file cluster being read
	uint16_t file_cur_pos;		// Current location in file (byte index)
	uint32_t file_size;			// Size of file being read, in bytes
	uint32_t global_cur_pos;	// Current location on disk
} f16_state;

/*!
 * Sets read to specific addr on disk
 */
void f16_disk_seek(uint32_t offset);

/*!
 * Reads a block of data from the disk to the read buffer.
 * Returns number of bytes read.
 *
 * This function provides the disk-level interface for the filesystem and must
 * be implimented by the user.
 */
uint16_t f16_disk_read(uint16_t count);

/*!
 * Writes a block of data from the disk from the write buffer.
 * Returns number of bytes written.
 *
 * This function provides the disk-level interface for the filesystem and must
 * be implimented by the user.
 */
uint16_t f16_disk_write(uint16_t count);


/*!
 * Reads partition table on disk.  Goes to first partition found.
 * Saves requried data from bootsector and partition table.
 * Sets read to start of root directory
 */
int f16_init(int parse_partitions);

/*!
 * Sets the read/write cursor to the specified position in the current file.
 */
void f16_seek_file(uint32_t position);

/*!
 * Opens the file of the given name in the directory.
 * Return 0 on success, -1 on file not found
 *
 * Read pointer should be at start of directory.
 * Filename must match the padded 8+3 format on disk,
 * eg log.txt becomes "LOG     TXT"
 */
int f16_open_file(char *filename);

/*!
 * Reads specified number of bytes from the current open file into read buffer.
 * Read head is moved to end of data read.
 * Returns number of bytes read.
 */
uint16_t f16_read_file(uint16_t bytes);


/*!
 *	Reads out the contents of the current directory
 */
int f16_readdir();


#endif
