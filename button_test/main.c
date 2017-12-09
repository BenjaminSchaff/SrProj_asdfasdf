#include "defines.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>


void init()
{
	DDRA |= (1<<4); // set PA4 to output (LED blink)

	// sd port switch (PB3)
	DDRB &= ~(1<<3);
	PORTB |= 1<<3;
	
	// button config
	MCUCR &= ~(1<<PUD); // disable pullup disable
	DDRC &= ~(0xFC); 	// PC2-PC7 set to input
	PORTC |= (0xFC);	// PC2-PC7 pullup enabled

	// Disable JTAG so PORTC works.  Yes, it needs both statements.	
	MCUCR = (1<<JTD);
	MCUCR = (1<<JTD);
	// And just to be sure...	
	MCUCR = (1<<JTD);
	MCUCR = (1<<JTD);
}


int main() 
{
	int state;	
	init();

	
	while (1) {
		if (~PINC & (1<<4))
			PORTA ^= 1<<4;

		if (~PINB & (1<<3))
			PORTA ^= 1<<4;

		_delay_ms(100);
	}
}
