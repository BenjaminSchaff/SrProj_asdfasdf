#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define SD_RW_BUF_SIZE 512

int main(int argc, char **argv)
{
	FILE *fin = fopen("test.img", "rb");
	char buffer[SD_RW_BUF_SIZE];

	uint32_t fat_start = 0x400C00;
	uint32_t data_start = 0x440000;
	uint32_t cluster_size = 512*128;
	uint16_t file_cluster = 5;
	uint32_t file_size = 131072;

	fseek(fin, data_start + (start_cluster-2)*cluster_size, SEEK_SET);
	fread(buffer, 1, sizeof(buffer), fin);

	buffer[sizeof(buffer) - 1] = 0;
	printf("%s",buffer);

	fclose(fin);

	return 0;
}
