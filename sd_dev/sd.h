#ifndef asdfasdf_sd
#define asdfasdf_sd

#include <stdint.h>

void cs_enable();

void cs_disable();

char sd_init();

uint8_t SPI_write_byte(uint8_t bytes);

/*
 *	Sends to the SD card the given command
 *	Returns the 1 bytes response
 */
uint8_t sd_send_command(
		uint8_t cmd,		// Command
		uint32_t arg, 		// 32 bit argument
		uint8_t crc); 		// CRC for cmd and arg

#endif


