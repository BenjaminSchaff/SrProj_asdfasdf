#include "defines.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include "uart.h"
#include "sensors.h"
#include "hd44780_settings.h"
#include "hd44780.h"

FILE uart_strm = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

char buf[20];


void print_sensors()
{
	int16_t temp = get_temp();
	unsigned long pressure = get_pressure();
	unsigned int wind = get_wind();
	uint16_t humid = get_humid();

	lcd_clrscr();
	lcd_home();

	sprintf(buf, "Temp: %d.%d C", temp/10, abs(temp%10)); // convert temp to a string
	lcd_puts(buf); // print temperature to first line of screen

	lcd_goto(0x40); // goto next line
	sprintf(buf, "Pres: %lu.%lu hPa", pressure/100, pressure%100); // convert pressure to a string
	lcd_puts(buf); // print pressure to second line of lcd

	lcd_goto(0x14);
	sprintf(buf, "Speed: %u.%u mph", wind/10, wind%10);
	lcd_puts(buf);

//	lcd_goto(0x54);
//	sprintf(buf, "Input Cap: %ld", aaaaa);
//	lcd_puts(buf);
}


void init()
{
	DDRA |= (1<<4); // set PA4 to output (LED blink)
	lcd_init();
	uart_init();
	i2c_init();
	sensor_init();
}

int main() 
{
	init();

	while (1) {
		PORTA ^= (1<<4); // Blinking LEDs are great. Also tells how fast main loop completes.

		update_sensors();
		
		print_sensors();
		_delay_ms(950);

		//fprintf(&uart_strm, "Temperature: %f C\n", f_temp);
//		fprintf(&uart_strm, "T: %d.%dC\n", c_temp/10, c_temp%10);
	//	fprintf(&uart_strm, "H: %d.%d%%\n", c_humid/10, c_humid%10);
//		fprintf(&uart_strm, "T2: %ld\n", bmp180_T);
//		fprintf(&uart_strm, "P: %ld\n", bmp180_P);	
	}
}
