#define F_CPU 1000000

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

int main() {

	DDRA |= (1<<4); // set PA4 to output
	while (1) {
		PORTA |= (1<<4);
		_delay_ms(500);
		PORTA &= ~(1<<4);
		_delay_ms(500);
	}
}
