#include <stdio.h>
#include <avr/io.h>
#define F_CPU 2000000
#include <util/delay.h>


int main() {
}

void command(char c)
{
	DDRA |= (1 << 5); // set PA5 to output
	PORTA &= ~(1 << 5); // clock enable falling edge
}


void write(char c)
{
	
	DDRA |= (1 << 5); // set PA5 to output
	PORTA &= ~(1 << 5); // clock enable falling edge
}

void init()
{
	DDRA |= (1 << 5); // set PA5 to output
	PORTA &= ~(1 << 5); // clock enable falling edge

	// three wakeup calls
	_delay_ms(50);
	command(0x30);
	_delay_ms(5);
	command(0x30);
	_delay_us(160);
	command(0x30);
	_delay_us(160);

	command(0x38); // set 8bit/2bit line
	command(0x10); // set cursor
	command(0x0C); // display ON and cursor ON
	command(0x06); // Entry mode set
}

void clear_display(char c)
{
	DDRD |= 0xFC; // set PD2-7 to outputs
	DDRA |= 0xD8; // set PA3, 5-7 to outputs

	PORTD &= ~(0xFC); // set DB2-7 to 0
	PORTD &= ~(0xC8); // set RS, R/W, and DB1 to 0
}
