/*! 
 * @file sd.h
 * 
 * @author Dan Paradis and Ben Schaff
 * 
 * Functions needed to communicate with SD card using the AVR through SPI.
 * (Still in developement. SD Init function not functional in testing)
 *
 */

#ifndef asdfasdf_sd
#define asdfasdf_sd

#include <stdint.h>

/*!
 * Enables SPI device by bringing SS pin low.
 * Also toggles LED/test pin PA4 if compiled as debug.
 */
void cs_enable();

/*!
 * Brings the SPI circuit select line high (inactive)
 */
void cs_disable();


/*!
 * Set SD card to SPI mode brigns card out of idle, sets proper block sizes
 */
char sd_init();

/*!
 *  Sends a byte out over the spi bus and returns the bytes recieved.
 *  Blocks until tranmission completes.
 */
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


