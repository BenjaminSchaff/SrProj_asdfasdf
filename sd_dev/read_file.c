#define DEBUG

#include <stdlib.h>
#include <stdint.h>
#ifdef DEBUG
	#include <stdio.h>
#endif

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


struct f16_entry { // sizeof()=32B
	uint8_t filename[8];
	uint8_t ext[3];
	uint8_t attributes;
	uint8_t reserved[10];
	uint16_t modify_time;
	uint16_t modify_date;
	uint16_t starting_cluster;
	uint32_t file_size;
} __attribute((packed));


uint8_t f16_r_buffer[32];	// read buffer used by fat16 disk io
uint8_t f16_w_buffer[512];	// write buffer for fat16 

/*
 * Minimum saved data for fat 16 operation
 */
struct {
	uint32_t fat_start; // = 0x400C00;		// Addr of FAT
	uint32_t root_start; //=?				// Addr of root dir
	uint32_t data_start; // = 0x440000;		// Addr of data sector
	uint32_t sect_per_cluster; // = 128;	// Number of sectors (512 bytes) per cluster
	uint16_t file_start_cluster;			// Index of starting cluster of file
	uint16_t file_cluster; // = 5;			// Index of file cluster being read
	uint32_t file_size; // = 131072;		// Size of file being read
	
} f16_state;

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
	struct f16_entry *entry = (struct f16_entry *)(struct f16_entry *)((void *)f16_r_buffer);

	f16_seek(0x1BE); // go to partition table start

	// Read partition table entries into buffer on at a time and find first valid one
	for ( i = 0; i < 4; i++ ) {	
		f16_read(sizeof(struct f16_part_table)); // read table entry

		// check if partition type, break if it's valid
		if (ptable->part_type == 4 
			|| ptable->part_type == 6
			|| ptable->part_type == 14) {
#ifdef DEBUG
			printf("FAT16 filesystem found from partition %d\n", i);
#endif
			break;
		}
	}
	

	if (i == 4) { // no valid fat16 partition found, return error
#ifdef DEBUG
		printf("No FAT16 filesystem found, exiting...\n");
#endif
		return -1;
	}

	// Need to save this before loading bootsector into buffer
	start_sector = ptable->start_sector; 

	// Read boot sector into buffer
	f16_seek(512 * ptable->start_sector);
	f16_read(sizeof(struct f16_boot_sector));
	
#ifdef DEBUG
	printf("Now at 0x%lX, sector size %d, FAT size %d sectors, %d FATs\n\n", 
		   ftell(fin), bootsect->sector_size, bootsect->fat_size_sects, bootsect->num_fats);
#endif		   
	
	// if sector size isn't 512, it won't work with SD card as implimented
	if (bootsect->sector_size != 512) {
		return -1;
	}

	// save important values to global f16_state struct
	f16_state.fat_start = (start_sector + bootsect->reserved_sectors) * 512;
	f16_state.root_start = f16_state.fat_start + bootsect->fat_size_sects
			* bootsect->num_fats * 512;
	f16_state.data_start = f16_state.root_start
			+ bootsect->root_dir_entries * sizeof(struct f16_entry);

#ifdef DEBUG
	printf("Fat start:\t%X\n", f16_state.fat_start);
	printf("Root start:\t%X\n", f16_state.root_start);
	printf("Data start:\t%X\n", f16_state.data_start);
#endif
	

	// Go to start of root dir
	f16_seek(f16_state.root_start);

#ifdef DEBUG
	printf("\nMoved to start of Root directory, now at 0x%lX\n", ftell(fin));
#endif




//////TODO ELSEWEHERER			
	for ( i = 0; i < bootsect->root_dir_entries; i++ ) {
		f16_read(sizeof(entry));
//	int j = 0;
		//TODO something with entry, find filenamme?  move this to openfile?
		//for (j
		//if (entry->filename[0] != 0x00 || entry-> 
		//printf("
	}
	
#ifdef DEBUG
	printf("\nRoot directory read, now at 0x%lX\n", ftell(fin));
#endif
	

	return 0;
}

void f16_seek_file(uint32_t position)
{
	//TODO traverses FAT for current file to specified position
}

void f16_read_file(uint16_t bytes)
{
	

	f16_seek(f16_state.data_start + (f16_state.file_start_cluster-2)*f16_state.sect_per_cluster*512);
	f16_read(bytes);

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

