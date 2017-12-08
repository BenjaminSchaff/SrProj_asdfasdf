#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct f16_part_table{
	uint8_t first_byte;
	uint8_t start_chs[3];
	uint8_t part_type;
	uint8_t end_chs[3];
	uint32_t start_sector;
	uint32_t length_sectors;
} __attribute((packed));

struct f16_boot_sector {
	//uini8_t jmp[3];
	//char oem[8];
	uint8_t start_padding[11]; // for allignment purposes
	uint16_t sect_sz;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t nfats;
	uint16_t root_dir_entries;
	uint16_t total_sectors_short; // if zero, later field is used
	uint8_t media_descriptor;
	uint16_t fat_sz_sects;
	/*
	uint16_t sectors_per_track;
	uint16_t nheads;
	uint32_t hidden_sectors;
	uint32_t total_sectors_long;
	
	uint8_t drive_number;
	uint8_t current_head;
	uint8_t boot_signature;
	uint32_t volume_id;
	char volume_label[11];
	char fs_type[8];
	char boot_code[448];
	uint16_t boot_sector_signature;*/
} __attribute((packed));


struct f16_entry {
	uint8_t filename[8];
	uint8_t ext[3];
	uint8_t attributes;
	uint8_t reserved[10];
	uint16_t modify_time;
	uint16_t modify_date;
	uint16_t starting_cluster;
	uint32_t file_size;
} __attribute((packed));


uint8_t f16_rw_buffer[512]; // read/write buffer used by fat16 disk io

/*
 * Minimum saved data for fat 16 operation
 */
struct f16_state {
	uint32_t fat_start; // = 0x400C00;		// Addr of FAT
	uint32_t data_start; // = 0x440000;		// Addr of data sector
	uint32_t sect_per_cluster; // = 128;	// Number of sectors (512 bytes) per cluster
	uint16_t file_start_cluster;			// Index of starting cluster of file
	uint16_t file_cluster; // = 5;			// Index of file cluster being read
	uint32_t file_size; // = 131072;		// Size of file being read
	
} __attribute((packed)); 

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
	return (uint16_t)fread(f16_rw_buffer, 1, count); //TODO read from SD
}


uint16_t f16_write(uint16_t count)
{
	// TODO write contents of buffer to disk
	return 0;
}

/* Reads partition table on disk.  Goes to first partition found.
 * Saves requried data from bootsector and partition table.
 * Sets read to start of root directory
 */
int f16_init()
{
	struct f16_part_table ptable[4];
	struct f16_boot_sector bootsect;
	struct f16_entry entry;

	f16_seek(0x1BE); // go to partition table start
	fread(ptable, sizeof(struct f16_part_table), 4, fin); // read all four entries
	
	for(i=0; i<4; i++) {		
		if(ptable[i].part_type == 4 || ptable[i].part_type == 6 ||
		   ptable[i].part_type == 14) {
			printf("FAT16 filesystem found from partition %d\n", i);
			break;
		}
	}
	
	if(i == 4) {
		printf("No FAT16 filesystem found, exiting...\n");
		return -1;
	}

	fseek(fin, 512 * ptable[i].start_sector, SEEK_SET);
	fread(&bootsect, sizeof(struct f16_boot_sector), 1, fin);
	
	printf("Now at 0x%lX, sector size %d, FAT size %d sectors, %d FATs\n\n", 
		   ftell(fin), bootsect.sect_sz, bootsect.fat_sz_sects, bootsect.nfats);
		   
	fseek(fin, (bootsect.reserved_sectors-1 + bootsect.fat_sz_sects * bootsect.nfats) *
		  bootsect.sect_sz, SEEK_CUR);
			
	for( i = 0; i < bootsect.root_dir_entries; i++ )
	{
		fread(&entry, sizeof(entry), 1, fin);
		print_file_info(&entry);
	}
	
	printf("\nRoot directory read, now at 0x%lX\n", ftell(fin));

	
	return 0;
}

void f16_seek_file(uint32_t position)
{
	//TODO traverses FAT for current file to specified position
}

void f16_read_file(uint16_t bytes)
{
	

	f16_seek(data_start + (start_cluster-2)*cluster_size, SEEK_SET);
	f16_read(bytes);

}


int main(int argc, char **argv) {
	int i;

	if (argc > 1) {
		fin = fopen(argv[1], "rb");
	} else {
		fin = fopen("test.img", "rb");
	}
	

	f16_init();

	f16_read_file(32);


	f16_rw_buffer[31] = 0;	
	f16_rw_buffer[512 - 1] = 0;
	printf("%s",buffer);


	fclose(fin);
	return 0;
}

