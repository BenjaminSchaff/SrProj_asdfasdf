/*!
 * @file f16_ds.c
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

#define DEBUG

#include <stdlib.h>
#include <stdint.h>
#ifdef DEBUG
	#include <stdio.h>
#endif

#include "f16_fs.h"

// structure of a fat16 partiotion table
struct f16_part_table { // sizeof()=16B
	uint8_t first_byte;
	uint8_t start_chs[3];
	uint8_t part_type;
	uint8_t end_chs[3];
	uint32_t start_sector;
	uint32_t length_sectors;
} __attribute((packed));

// structure of fat16 bootsector
struct f16_boot_sector { //sizeof()=24B
	//uini8_t jmp[3];
	//char oem[8];
	uint8_t start_padding[11]; // for allignment purposes
	uint16_t sector_size;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t num_fats;
	uint16_t root_dir_entries;
	uint16_t total_sectors_short; // if zero, later field is used
	uint8_t media_descriptor;
	uint16_t fat_size_sects;
	/* Not needed for SD card
	uint16_t sectors_per_track;
	uint16_t nheads;
	uint32_t hidden_sectors;
	uint32_t total_sectors;
	
	uint8_t drive_num;
	uint8_t current_head;
	uint8_t boot_sig;
	uint32_t volume_id;
	uint8_t volume_label[11];
	uint8_t fs_type[8];
	uint8_t boot_code[448];
	uint16_t boot_sect_sig;
	*/
} __attribute((packed));

// structure of fat16 file
struct f16_file { // sizeof()=32B
	uint8_t filename[11];
	uint8_t attri;
	uint8_t res[10];
	uint16_t mod_time;
	uint16_t mod_date;
	uint16_t start_cluster;
	uint32_t file_size;
} __attribute((packed));


uint8_t f16_r_buffer[32];	// read buffer used by fat16 disk io
uint8_t f16_w_buffer[512];	// write buffer for fat16 


/* 
 * Reads partition table on disk.  Goes to first partition found.
 * Saves requried data from bootsector and partition table.
 * Sets read to start of root directory
 */
int f16_init(int parse_partitions)
{
	int i;
	uint32_t start_sector;
	struct f16_part_table *ptable = (struct f16_part_table *)((void *)f16_r_buffer);
	struct f16_boot_sector *bootsect = (struct f16_boot_sector*)((void *)f16_r_buffer);


	if (parse_partitions == 1) {
		f16_disk_seek(0x1BE); // go to partition table start

		// Read partition table entries into buffer on at a time and find first valid one
		for (i = 0; i < 4; i++) {	
			f16_disk_read(sizeof(struct f16_part_table)); // read table entry

			// check if partition type, break if it's valid
			if (ptable->part_type == 4 
				|| ptable->part_type == 6
				|| ptable->part_type == 14) {
				break;
			}
		}
		
		if (i == 4) { // no valid fat16 partition found, return error
			start_sector = 0; // actually, lets just assume there was no partition table for now.
			//return -1;
		} else {
			// Need to save this before loading bootsector into buffer
			start_sector = ptable->start_sector; 
		}
	} else {
		start_sector = 0;
	}

	// Read boot sector into buffer
	f16_disk_seek(512 * start_sector);
	f16_disk_read(sizeof(struct f16_boot_sector));
	

	// if sector size isn't 512, it won't work with SD card as implimented
	if (bootsect->sector_size != 512) {
		return -1;
	}

	// save important values to global f16_state struct
	f16_state.fat_start = (start_sector + bootsect->reserved_sectors) * 512;
	f16_state.root_start = f16_state.fat_start + bootsect->fat_size_sects
			* bootsect->num_fats * 512;
	f16_state.data_start = f16_state.root_start
			+ bootsect->root_dir_entries * sizeof(struct f16_file);

	f16_state.sect_per_cluster = bootsect->sectors_per_cluster;
	f16_state.file_start_cluster = 0xFFFF;	// no fat lookup on root dir
	f16_state.file_cur_cluster = 0xFFFF;		// no fat lookup on root dir
	f16_state.cluster_left = 0xFFFFFFFF;
	f16_state.file_size = bootsect->root_dir_entries * sizeof(struct f16_file);
	f16_state.file_cur_pos = 0;
	
#ifdef DEBUG
	printf("Fat start:\t%X\n", f16_state.fat_start);
	printf("Root start:\t%X\n", f16_state.root_start);
	printf("Data start:\t%X\n", f16_state.data_start);
#endif
	

	// Go to start of root dir
	f16_disk_seek(f16_state.root_start);

	return 0;
}


/*!
 * ?
 */
void f16_seek_file(uint32_t position)
{
	int clusters_into_file, i;
	
	//TODO traverses FAT for current file to specified position

	// Don't allow movement past end of file
	if (position > f16_state.file_size) {
		position = f16_state.file_size;
	} 	
	f16_state.file_cur_pos = position;


	clusters_into_file = f16_state.file_cur_pos / (f16_state.sect_per_cluster * 512);
	for(i = 0; i < clusters_into_file; i++) {
		// go to FAT
		f16_disk_seek(f16_state.fat_start + f16_state.file_cur_cluster*2); // current entry
		f16_disk_read(2);	// read current to get num of next
		
		f16_state.file_cur_cluster = f16_r_buffer[0] | f16_r_buffer[1]<<1;

		if (f16_state.file_cur_cluster == 0xFFFF) { // end of chain
			f16_state.file_cur_pos = f16_state.file_size;
			return;
		}
	}
/*
	//TODO, this will break on FAT traversal
	f16_state.global_cur_pos = f16_state.data_start + ((f16_state.file_start_cluster-2)
			* f16_state.sect_per_cluster*512) + f16_state.file_cur_pos;
*/
	f16_state.global_cur_pos = f16_state.data_start + ((f16_state.file_cur_cluster-2)
			* f16_state.sect_per_cluster*512) 
			+ (f16_state.file_cur_pos % (f16_state.sect_per_cluster * 512));


	f16_disk_seek(f16_state.global_cur_pos);

/*
#ifdef DEBUG 	
	printf("file position 0x%X\n", position);
	printf("file start cluster 0x%X\n", f16_state.file_start_cluster);
	printf("sect per cluster 0x%X\n", f16_state.sect_per_cluster);
	printf("global position 0x%X\n", f16_state.global_cur_pos);
	printf("global position 0x%X\n", f16_state.global_cur_pos);
#endif 
*/
}


/*!
 * Opens the file of the given name in the directory.
 * Return 0 on success, -1 on file not found
 *
 * Read pointer should be at start of directory.
 * Filename must match the padded 8+3 format on disk,
 * eg log.txt becomes "LOG     TXT"
 */
int f16_open_file(char *filename)
{
	struct f16_file *entry = (struct f16_file *)((void *)f16_r_buffer);
	int i, j;
	
	//TODO return to root dir
	// if filename[0] == '/'
	//	set stuff back to original

	//TODO, make subdirs actually work

	for (i= 0; i < 512; i++) {
		// read entry
		f16_disk_read(sizeof(struct f16_file));


		// check file type
		switch (entry->filename[0]) {
		case 0x00:	// empty entry, move on
		case 0x2E:	// . or ..
		case 0xE5:	// Deleted file
			continue;
		default:	// Must be an actual file, right?
			break;
		}
		
		if (entry->attri & (0x04) ) {
			//system file, or reserved, or something don't touch
			continue;
		} else if (entry->attri & 0x10) {
#ifdef DEBUG
//		printf("Dir\t%.11s\n", entry->filename);
#endif
		} else {
#ifdef DEBUG
//		printf("File\t%.11s\n", entry->filename);
#endif
		}

		// compare filename
		for (j = 0; j < 11; j++) {
			if (filename[j] != entry->filename[j]) { // Filename different
				break;
			}
		}
		if (j < 11) {	// Not the right file
			continue;
		}

		
		// stuff todo with aactually opening file, setting size, pos, offset, etc
		f16_state.file_cur_pos = 0;
		f16_state.file_start_cluster = entry->start_cluster;
		f16_state.file_cur_cluster = entry->start_cluster;
		f16_state.cluster_left = f16_state.sect_per_cluster * 512;
		
		f16_state.file_size = entry->file_size; //TODO, someything different here for subdir

		f16_seek_file(0);

#ifdef DEBUG
//		printf("File start cluster:\t%X\n",f16_state.file_start_cluster);
//		printf("Read pos:\t%X\n",f16_state.global_cur_pos);
//		printf("Read pos:\t%X\n",f16_state.global_cur_pos);
#endif
		return 0;
	}

	return -1;
}


/*!
 * Reads specified number of bytes from the current open file into read buffer.
 * Read head is moved to end of data read.
 * Returns number of bytes read.
 */
uint16_t f16_read_file(uint16_t bytes)
{
	// check end of file
	if (f16_state.file_cur_pos >= f16_state.file_size) {
		return 0;
	}
	
	// check if at the end of the cluster
	if (f16_state.cluster_left == 0) {
		f16_seek_file(f16_state.file_cur_pos);		
	}

	// check end of file/cluster
	if (bytes > (f16_state.file_size - f16_state.file_cur_pos))
		bytes = f16_state.file_size - f16_state.file_cur_pos;
	if (bytes > f16_state.cluster_left)
		bytes = f16_state.cluster_left;

	// read bytes from disk
	bytes = f16_disk_read(bytes);

	f16_state.cluster_left -= bytes;

	// move cursor to byte after ones read
	f16_seek_file(f16_state.file_cur_pos + bytes);


	return bytes;
}

/*!
 *  Reads out the contents of the current directory
 */
int f16_readdir()
{
	struct f16_file *entry = (struct f16_file *)((void *)f16_r_buffer);
	int i;
	
	//TODO, error out if called when not in dir
	//TODO, make this return an array of strings or something
#ifdef DEBUG
	printf("Directory Contents\n");
#endif
	for (i= 0; i < 512; i++) {
		// read entry
		f16_disk_read(sizeof(struct f16_file));

		// check file type
		switch (entry->filename[0]) {
		case 0x00:	// empty entry, move on
		case 0x2E:	// . or ..
		case 0xE5:	// Deleted file
			continue;
		default:	// Must be an actual file, right?
			break;
		}
		
		if (entry->attri & (0x04) ) {
			//system file, or reserved, or something don't touch
			continue;
		} else if (entry->attri & 0x10) {
#ifdef DEBUG
		printf("Dir\t%.11s\n", entry->filename);
#endif
		} else {
#ifdef DEBUG
			printf("File\t%.11s\n", entry->filename);
#endif
		}
	}
	// Navigate read head back to start of dir
	//TODO, I dont think this works with anything but root dir
	f16_disk_seek(
		f16_state.root_start 
//		+ ((f16_state.file_start_cluster-2) * f16_state.sect_per_cluster*512)
	);
	return 0;
}

/*!
 * Tests for end of file
 *
 * Returns a non-zero value if the read/write pointer has reached end of the
 * file; otherwise it returns a zero.
 */
int f16_eof()
{
	// check end of file
	if (f16_state.file_cur_pos >= f16_state.file_size) {
		return 1;
	} else {
		return 0;
	}
}
