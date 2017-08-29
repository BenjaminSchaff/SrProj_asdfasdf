#include <stdio.h>
#include <avr/io.h>
#define F_CPU 1000000
#include <util/delay.h>


int main() {
	DDRB |= (1<<0); // set PB0/D0  to output
	while (1) {
		PORTB |= (1<<0);
		_delay_ms(250);
		PORTB &= ~(1<<0);
		_delay_ms(250);
	}
}
