#include "defines.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include "uart.h"
#include "ui.h"
#include "i2c.h"
#include "sensors.h"
#include "hd44780_settings.h"
#include "hd44780.h"

FILE uart_strm = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

char lcd_buf[21];

void sensor_readout_update()
{
	int16_t temp = get_temp();
	unsigned long pressure = get_pressure();
	unsigned int wind = get_wind();
	uint16_t humid = get_humid();

	sprintf(buf, "Temp: %d.%d C", temp/10, abs(temp%10)); // convert temp to a string
	lcd_puts(buf); // print temperature to first line of screen

	lcd_goto(0x40); // goto next line
	sprintf(buf, "Pres: %lu.%lu hPa", pressure/100, pressure%100); // convert pressure to a string
	lcd_puts(buf); // print pressure to second line of lcd

	lcd_goto(0x14);
	sprintf(buf, "Speed: %u.%u mph", wind/10, wind%10);
	lcd_puts(buf);
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
	int button, current_screen_index = 0;
	int i;
	SCREEN ui[3];

	init();
	ui_init(ui);

	while (1) {
		PORTA ^= (1<<4); // Blinking LEDs are great. Also tells how fast main loop completes.

		print_screen(&ui[current_screen_index]); // update screen

		for (i = 1; i < 7; i++) { // buttons are labeled 1-6
			if (((~PINC) << (1 + i)) & 0x01) {
				if (i == 6) { // if the button is back screen, go home
					current_screen_index = 0; // 0 is the home screen
				} else if (i == 3) { // if you press the goto screen button
					if (current_screen == 0) { // and you are at the home screen
						if (ui[0].curser_index == 0) // and your curser is over the sensors option
							current_screen_index = 1; // goto sensors
						else if (ui[0].curser_index == 2) // and your curser is over the settings option
							current_screen_index = 2; // goto settings
					}
				}
				update_screen_state(i, current_screen, &ui[current_screen_index]); // update screens with new values
				break;
			}
		}
		update_sensors();
		switch (current_screen_index) {
		case 1:
			update_sensor_strings(&ui[1], &ui[2], lcd_buf);
			break;
		case 2:
			update_settings_strings(&ui[2]);
			break;
		}
		
		_delay_ms(950);

		//fprintf(&uart_strm, "Temperature: %f C\n", f_temp);
//		fprintf(&uart_strm, "T: %d.%dC\n", c_temp/10, c_temp%10);
	//	fprintf(&uart_strm, "H: %d.%d%%\n", c_humid/10, c_humid%10);
//		fprintf(&uart_strm, "T2: %ld\n", bmp180_T);
//		fprintf(&uart_strm, "P: %ld\n", bmp180_P);	
	}
}
