#include "defines.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>

#include "uart.h"
#include "i2c.h"

FILE uart_strm = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void init()
{
	DDRA |= (1<<4); // set PA4 to output (LED blink)

	uart_init();
}


void i2c_write_test()
{
	i2c_start();
	
//	i2c_write_byte((TEMP_SENSE_ADDR<<1) | 0x00);
	i2c_write_byte((PRES_SENSE_ADDR<<1) | 0x00);
	i2c_write_byte(0xFF);
	i2c_stop();
/*	
	if (val != 0)
		fprintf(&uart_strm, "n%x", val);
	else 
		fprintf(&uart_strm, "y\1");*/
}

int main() 
{
	init();
	

	while (1) {
		PORTA |= (1<<4);
		_delay_ms(100);
		PORTA &= ~(1<<4);
		_delay_ms(100);

		i2c_write_test();

		fprintf(&uart_strm, "test\n");
	}
}
