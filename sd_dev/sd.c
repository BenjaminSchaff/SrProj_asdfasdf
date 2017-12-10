#include "defines.h"

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "sd.h"

#define SD_DEBUG // if defined, PA4 is toggled the same as the CS line

#define RETRY_COUNT 100
#define RETRY_DELAY 10
/*
 *	Brings the SPI circuit select line low (active)
 */
void cs_enable()
{
	PORTB &= ~(1<<4);   // CS Enable
#ifdef SD_DEBUG
	// for debugging also, also toggling led pin which is broken out
	PORTA &= ~(1<<4);
#endif

}

/*
 *	Brings the SPI circuit select line high (inactive)
 */
void cs_disable()
{
	PORTB |= (1<<4);    // CS Disable
#ifdef SD_DEBUG
	// for debugging bus, also toggling led pin which is broken out
	PORTA |= (1<<4);
#endif
}

/*
 *	Performes SPI pin and clock initialization
 */
void spi_init()
{
	// Setting up SPI pins
	DDRB |= (1<<4) | (1<<5) | (1<<7);	// Set CS (PB4), MOSI (PB5),and SCK (PB7) output.  
	DDRB &= ~(1<<6);	// and MISO (PB6) as input
	PORTB |= (1<<6);	// Pullup on MISO	
	// Enable SPI, master mode, 
	//SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0) | (1<<SPR1);
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR1);
}

/*
 *	Set SD card to SPI mode brigns card out of idle, sets proper block sizes
 */
char sd_init()
{
	int i;
	uint8_t ret;

	spi_init(); // SD card needs SPI working first


	cs_disable(); // Begin communication

	// send 80 clock cycles so card can sync up
	for (i = 0; i < 10; i++) {
		SPI_write_byte(0xFF);
	}


	// Send CMD0 to enter SPI mode until we get "0x01" back
	for (i = 0; i < RETRY_COUNT; i++) {
		if (sd_send_command(0x40, 0x00000000, 0x95) == 0x01) { // needs valid crc (or not)
			break;
		}
		_delay_ms(RETRY_DELAY);
	}
	if (i == RETRY_COUNT) return -1; // no response. You will not use SD card today


	// Send CMD 8
	for (i = 0; i < RETRY_COUNT; i++) {
		ret = sd_send_command(0x40|8, 0x00000000, 0xFF); // doesn't need valid crc
		if (ret != 0xFF) {
			break;
		}
		_delay_ms(RETRY_DELAY);
	}

	// ACMD41 = CMD55 (0x77) and then CMD41	(0x69)
	// Send ACMD41 to come out of idle, retry until we get "0x00" back
	for (i = 0; i < RETRY_COUNT; i++) {
		ret = sd_send_command(0x40|55, 0x00000000, 0xFF); // doesn't need valid crc
		ret = sd_send_command(0x40|41, 0x40000000, 0xFF); // doesn't need valid crc
		if (ret == 0x00) {
			break;
		}
		_delay_ms(RETRY_DELAY);
	}
	
	//return ret;

	
	// Send CMD1 to come out of idle, retry until we get "0x00" back
	for (i = 0; i < RETRY_COUNT; i++) {
		ret = sd_send_command(0x41, 0x00000000, 0xFF); // doesn't need valid crc
		if (ret == 0x00) {
			break;
		}
		_delay_ms(RETRY_DELAY);
	}
	if (i == RETRY_COUNT) return ret; //-2; // no response. You will not use SD card today
	/*	
	do {
		ret = sd_send_command(0x41, 0x00000000, 0xFF); // doesn't need valid crc
	} while (ret == 0x01);
	return ret;
*/

	
	// Send CMD16 to set blocksize to 512 for FAT16
	sd_send_command(0x50, 0x00000200, 0xFF);


	//TODO, whatever sector bookkeeping we need
	return 0;	
}

/*
 *	Sends a byte out over the spi bus and returns the bytes recieved.
 * 	Blocks until tranmission completes.
 */
uint8_t SPI_write_byte(uint8_t byte) 
{
	SPDR = byte;					// load byte into spi data register
	while (!(SPSR & (1<<SPIF)));	// wait until transmission completes
	return SPDR;					// return byte recieved
}

/*
 *	Sends to the SD card the given command
 *	Returns the 1 bytes response
 */
uint8_t sd_send_command(
		uint8_t cmd,		// Command
		uint32_t arg, 		// 32 bit argument
		uint8_t crc) 		// CRC for cmd and arg
{
	uint8_t i;
	uint8_t response[8];

	cs_enable(); // Begin SPI transmision

	SPI_write_byte(cmd);
	SPI_write_byte((arg>>24) & 0xFF);	// send MSB of argument
	SPI_write_byte((arg>>16) & 0xFF);	// and next MSB
	SPI_write_byte((arg>>8) & 0xFF);	// and next...
	SPI_write_byte(arg & 0xFF);			// and LSB
	SPI_write_byte(crc);

	// read response.  Response comes 0-8 bytes after sending command.
	// Need to read all 8
	for (i = 0; i < 8; i++) {
		response[i] = SPI_write_byte(0xFF);
	}
	
	cs_disable(); // End SPI transmission
	
	// Find which byte was the response
	for (i =0; i < 8; i++) {
		if (response[i] != 0xFF) {
			return response[i];
		}
	}
	
	// no response
	return 0xFF; // basically -1
}
