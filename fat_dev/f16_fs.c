// file f16_fs.c
#define DEBUG

#include <stdlib.h>
#include <stdint.h>
#ifdef DEBUG
	#include <stdio.h>
#endif

#include "f16_fs.h"

struct f16_part_table { // sizeof()=16B
	uint8_t first_byte;
	uint8_t start_chs[3];
	uint8_t part_type;
	uint8_t end_chs[3];
	uint32_t start_sector;
	uint32_t length_sectors;
} __attribute((packed));


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
int f16_init()
{
	int i;
	uint32_t start_sector;
	struct f16_part_table *ptable = (struct f16_part_table *)((void *)f16_r_buffer);
	struct f16_boot_sector *bootsect = (struct f16_boot_sector*)((void *)f16_r_buffer);

	f16_seek(0x1BE); // go to partition table start

	// Read partition table entries into buffer on at a time and find first valid one
	for (i = 0; i < 4; i++) {	
		f16_read(sizeof(struct f16_part_table)); // read table entry

		// check if partition type, break if it's valid
		if (ptable->part_type == 4 
			|| ptable->part_type == 6
			|| ptable->part_type == 14) {
			break;
		}
	}
	
	if (i == 4) { // no valid fat16 partition found, return error
		return -1;
	}

	// Need to save this before loading bootsector into buffer
	start_sector = ptable->start_sector; 

	// Read boot sector into buffer
	f16_seek(512 * ptable->start_sector);
	f16_read(sizeof(struct f16_boot_sector));
	

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

	f16_state.file_start_cluster = 0xFFFF;	// no fat lookup on root dir
	f16_state.file_cur_cluster = 0xFFFF;		// no fat lookup on root dir
	f16_state.file_size = bootsect->root_dir_entries * sizeof(struct f16_file);
	f16_state.file_cur_pos = 0;
	
#ifdef DEBUG
	printf("Fat start:\t%X\n", f16_state.fat_start);
	printf("Root start:\t%X\n", f16_state.root_start);
	printf("Data start:\t%X\n", f16_state.data_start);
#endif
	

	// Go to start of root dir
	f16_seek(f16_state.root_start);

	return 0;
}

void f16_seek_file(uint32_t position)
{
	//TODO traverses FAT for current file to specified position

	// Don't allow movement past end of file
	if (position > f16_state.file_size) {
		position = f16_state.file_size;
	} 	
	f16_state.file_cur_pos = position;


	f16_state.global_cur_pos = f16_state.data_start + ((f16_state.file_start_cluster-2)
			* f16_state.sect_per_cluster*512) + f16_state.file_cur_pos;

	f16_seek(f16_state.global_cur_pos);
}


/* 
 * Read pointer should be at start of 
 * Opens the file with the given name.
 * Filename must match the padded 8+3 format on disk, 
 * eg log.txt becomes "LOG     TXT"
 * Return 0 on success, -1 on file not found
 */
int f16_open_file(char *filename)
{
	struct f16_file *entry = (struct f16_file *)((void *)f16_r_buffer);
	int i, j;
	
	//TODO return to root dir
	// if filename[0] == '/'
	//	set stuff back to original

	//TODO, add support for subdirs

	for (i= 0; i < 512; i++) {
		// read entry
		f16_read(sizeof(struct f16_file));


		// check file type
		switch (entry->filename[0]) {
		case 0x00:	// empty entry, move on
		case 0x2E:	// directory (not implimented)
		case 0x5E:	// Deleted file
			continue;
		default:	// Must be an actual file, right?
			break;
		}
		
#ifdef DEBUG
		printf("%.11s\n", entry->filename);
#endif
		

		// compare filename
		for (j = 0; j < 11; j++) {
			if (filename[j] != entry->filename[j]) { // Filename different
				break;
			}
		}
		if (j < 11) {	// Not the right file
			continue;
		}

#ifdef DEBUG
		printf("Successfully opened file %s\n", filename);
#endif
		
		//TODO stuff with aactually opening file, setting size, pos, offset, etc
		f16_state.file_cur_pos = 0;
		f16_state.file_start_cluster = entry->start_cluster;
		f16_state.file_size = entry->file_size;
		
		// current filesize limited to one cluster
		//TODO not do this. 
		return 0;
	}

	return -1;
}


uint16_t f16_read_file(uint16_t bytes)
{
	// check end of file
	if (f16_state.file_cur_pos >= f16_state.file_size) {
		return 0;
	}
	
	//TODO support files larger than one cluster (64kB) by actually using FAT
/*
	// check end of cluster
	if (f16_state.file_cur_pos %
*/

	// read bytes from disk
	bytes = f16_read(bytes);

	// move cursor to byte after ones read
	f16_seek_file(f16_state.file_cur_pos + bytes);


	return bytes;
}

