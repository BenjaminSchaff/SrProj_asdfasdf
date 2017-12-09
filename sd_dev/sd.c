#include <stdlib.h>
#include <avr/io.h>
#include "sd.h"

void sd_init()
{

	// Setting up SPI pins
	DDRB |= (1<<4) | (1<<5) | (1<<7);	// Set CS (PB4), MOSI (PB5),and SCK (PB7) output.  
	DDRB &= ~(1<<6);	// and MISO (PB6) as input
	PORTB |= (1<<6);	// Pullup on MISO
	
	// Enable SPI, master mode, 
	SPCR = (1<<SPE) | (1<<MSRR) | (1<<SPR0) | (SPR1);
}


uint8_t SPI_write_char(uint8_t byte) 
{
	SPDR = byte;
	while (!(SPSR & (1<<SPIF)));
	return SPDR;
}
