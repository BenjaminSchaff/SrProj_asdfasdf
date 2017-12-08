#include "defines.h"

#include <avr/io.h>
#include <stdlib.h>

#include "ui.h"
#include "hd44780_settings.h"
#include "hd44780.h"
#include "sensors.h"

void update_screen_state(int button, int current_screen_index, SCREEN *current_screen)
{
	int ret = current_screen_index;

	if (button == 1) { // if the right arrow key is pressed
		current_screen->line_values[current_screen->curser_index]++; // increment value of line
		
		// check if the line value is overflowed
		if (current_screen->line_values[current_screen->curser_index] >= current_screen->max_values[current_screen->curser_index])
			current_screen->line_values[current_screen->curser_index] = 0; // loop back to 0

	} else if (button == 5) { // if the left arrow key is pressed
		current_screen->line_values[current_screen->curser_index]--; // decrement value of line

		// check if the line value goes below 0
		if (current_screen->line_values[current_screen->curser_index] < 0)
			current_screen->line_values[current_screen->curser_index] = current_screen->max_values[current_screen->curser_index];

	} else if (button == 2) { // if the up arrow key is pressed
		if (current_screen->screen_index == current_screen->curser_index) { // if the curser is at the top of the screen
			if (current_screen->curser_index != 0) { // if the curser is not
				current_screen->screen_index--; // decrement both the curser and screen index
				current_screen->curser_index--;
			}
		} else { // if the curser is not at the top of the screen
			current_screen->curser_index--; // decrement the curser index
		}
	} else if (button == 4) { // if the down key is pressed
		if ((current_screen->screen_index + 3) == current_screen->curser_index) { // if the curser is at the bottom of the screen
			if (current_screen->curser_index != (current_screen->length + 1)) { // if the curser has not hit the bottom of the list
				current_screen->screen_index++; // increment screen and curser index
				current_screen->curser_index++;
			}
		} else {
			current_screen->curser_index++; // increment curser index
		}
	} 
}

void print_screen(SCREEN *current_screen)
{
	lcd_clrscr();
	lcd_puts((current_screen->lines)[screen_index]);
	lcd_goto(0x40);
	lcd_puts((current_screen->lines)[screen_index + 1]);
	lcd_goto(0x14);
	lcd_puts((current_screen->lines)[screen_index + 2]);
	lcd_goto(0x54);
	lcd_puts((current_screen->lines)[screen_index + 3]);

	switch (current_screen->curser_index - current_screen->screen_index) {
	case 0:
		lcd_home();
		break;
	case 1:
		lcd_goto(0x40);
		break;
	case 2:
		lcd_goto(0x14);
		break;
	case 3:
		lcd_goto(0x54);
		break;
	}
}

void update_settings_strings(SCREEN *s)
{
	int i;
	
	switch (s->line_values[0]) { // check what value temp is
	case 0:
		s->lines[0] = "Temp Units: C"; // choose C or F depending on it
		break;
	case 1:
		s->lines[0] = "Temp Units: F";
		break;
	}

	switch (s->line_values[1]) { // check what value Pressure should be
	case 0:
		s->lines[1] = "Press Units: hPa"; // choose hPa, atm, or bars
		break;
	case 1:
		s->lines[1] = "Press Units: psi";
		break;
	case 2:
		s->lines[1] = "Press Units: bars";
		break;
	}

	switch (s->line_values[2]) { // check what value wind should be
	case 0:
		s->lines[2] = "Wind Units: MPH"; // pick mph, kph, m/s, or ft/s depending on the value
		break;
	case 1:
		s->lines[2] = "Wind Units: KPH";
		break;
	case 2:
		s->lines[2] = "Wind Units: ft/s";
		break;
	case 3:
		s->lines[2] = "Wind Units: m/s";
		break;
	}

	if (s->line_values[3] == 2) { // reset everything to zero
		s->lines[0] = "Temp Units: C";
		s->lines[1] = "Press Units: hPa";
		s->lines[2] = "Wind Units: MPH";
		s->line_values[0] = 0;
		s->line_values[1] = 0;
		s->line_values[2] = 0;
		s->line_values[3] = 0;
	}
}

void update_sensor_strings(SCREEN *sensors, SCREEN *settings)
{
	int i;

	for (i = 0; i < sensors->length; i++) {
		
	}
}

/* Changes units for temperature in accordance to what the settings say        */
/* Returns the string that should be saved in the sensor screen's lines array */
char *get_temp_string(SCREEN *settings)
{
	char ret[20];
	int16_t temp = get_temp();

	if (settings->line_values[0] == 0) { // if temp setting says to use Fahrenheit
		sprintf(ret, "Temp: %d.%d C", temp/10, abs(temp%10)); // convert temp to a string
	} else { // Temp units should be in Fahrenheit
		temp = (temp * 9)/5 + 320; // convert temp to fahrenheit
		sprintf(ret, "Temp: %d.%d F", temp/10, abs(temp%10));
	}
	
	return ret;
}

/* Changes units for pressure in accordance to what the settings say          */
/* Returns the string that should be saved in the sensor screen's lines array */
char *get_pressure_string()
{
	char ret[20];
	unsigned long pressure = get_pressure();

	if (settings->line_values[1] == 0) { // if setting is 0 use hPa as unit
		sprintf(ret, "Pres: %lu.%lu hPa", pressure/100, pressure%100); // print to ret, the pressure in hPa
	} else if (settings->line_values[1] == 1) {
		pressure = (pressure * 10) / 6894 ; // convert to psi
		sprintf(ret, "Pres: %lu.%lu psi", pressure/10, pressure%10);
	} else { // convert to bars
		pressure = (pressure * 100) / 10000; // the * 100 is to preserve sig figs
		if (press < 1000)
			sprintf(ret, "Pres: 0.%lu bars", pressure);
		else
			sprintf(ret, "Pres: 1.%lu bars", (pressure/10)%10);
	}

	return ret;
}

/* Changes units for wind speed in accordance to what the settings say        */
/* Returns the string that should be saved in the sensor screen's lines array */
char *get_wind_string()
{
	char ret[20];
	unsigned int wind = get_wind();

	if (settings->line_values[2] == 0) { // print as mph
		sprintf(ret, "Wind: %u.%u mph", wind/10, wind%10);
	} else if (settings->line_values[2] == 1) {
		wind = wind * 1.61; // convert to kph
		sprintf(ret, "Wind: %u.%u kph", wind/10, wind%10);
	} else if (settings->line_values[2] == 2) { // convert to ft/s
		wind = wind * 1.46667;
		sprintf(ret, "Wind: %u.%u ft/s", wind/10, wind%10);
	} else {
        wind = (wind * 10) / 2.236; // convert to m/s and keep sig figs
        sprintf(ret, "Wind: %u.%u m/s", wind/100, wind%100);
	}
}
