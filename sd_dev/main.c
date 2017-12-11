/*! 
 * @file sd_dev.c
 * 
 * @author Dan Paradis and Ben Schaff
 * 
 * Program for testing code to communicate with SD card using the AVR
 *
 */
#include "defines.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include "sd.h"
#include "hd44780_settings.h"
#include "hd44780.h"

char lcd_buf[21];
uint8_t read_buffer[32];

// Stalls until any button is pressed.
void wait_for_key()
{
	// Buttons are active low, wait here until one is pressed.	
	while((PINC & 0xFC) == 0xFC)
		_delay_ms(50);
}


void do_sd_init()
{
	char ret;
	// do sd init last
	ret = sd_init();

	
	sprintf(lcd_buf, "SD init ret %d", ret);
	lcd_clrscr();
	lcd_home();	
	lcd_puts(lcd_buf);


}

void init()
{
	DDRA |= (1<<4); // set PA4 to output (LED blink)

	// button config
	MCUCR &= ~(1<<PUD); // disable pullup disable
	DDRC &= ~(0xFC);	// PC2-PC7 set to input
	PORTC |= (0xFC);	// PC2-PC7 pullup enabled

	// Disable JTAG so PORTC works.  Yes, it needs both statements.
	MCUCR = (1<<JTD);
	MCUCR = (1<<JTD);
	// And just to be sure...
	MCUCR = (1<<JTD);
	MCUCR = (1<<JTD);


	lcd_init();
	
	lcd_clrscr();
	lcd_home();
	sprintf(lcd_buf, "Insert SD Card and");
	lcd_puts(lcd_buf);
	lcd_goto(0x40); // goto next line
	sprintf(lcd_buf, "then press any key");	
	lcd_puts(lcd_buf);
	wait_for_key();


	do_sd_init();
}



int main() 
{
	uint8_t buf2[32];

	buf2[0] = buf2[1] = buf2[2] = 0;


	init();

	while (1) {

		if (~PINC & (1<<4)) {
			cs_enable();
			buf2[0] = SPI_write_byte(0xAA);
			buf2[1] = SPI_write_byte(0xF0);
			buf2[2] = SPI_write_byte(0xCF);
			cs_disable();
		}

		if (~PINC & (1<<5)) {
			do_sd_init();

		}

	
		PORTA ^= (1<<4); // Blinking LEDs are great. Also tells how fast main loop completes.
		_delay_ms(150);

	}
}
