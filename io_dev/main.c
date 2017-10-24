#define F_CPU 1000000

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include <util/twi.h>

#include "uart.h"

FILE uart_strm = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void init()
{
	// LED blinker
	DDRA |= (1<<4); // set PA4 to output

	uart_init();
}

int main() 
{
	init();
	

	while (1) {
		PORTA |= (1<<4);
		_delay_ms(100);
		PORTA &= ~(1<<4);
		_delay_ms(100);
		fprintf(&uart_strm, "test\n");
	}
}
