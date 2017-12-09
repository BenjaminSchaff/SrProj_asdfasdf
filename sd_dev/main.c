#include "defines.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

void init()
{
	DDRA |= (1<<4); // set PA4 to output (LED blink)
}

int main() 
{
	init();

	while (1) {
		PORTA ^= (1<<4); // Blinking LEDs are great. Also tells how fast main loop completes.

		
		_delay_ms(950);

	}
}
