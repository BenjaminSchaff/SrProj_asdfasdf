#include "defines.h"

#include <avr/io.h>

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
	
	switch (s->line_values[0]) {
	case 0:
		s->lines[0] = "Temp Units: C";
		break;
	case 1:
		s->lines[0] = "Temp Units: F";
		break;
	}

	switch (s->line_values[1]) {
	case 0:
		s->lines[1] = "Press Units: hPa";
		break;
	case 1:
		s->lines[1] = "Press Units: atm";
		break;
	case 2:
		s->lines[1] = "Press Units: bars";
		break;
	}

	switch (s->line_values[2]) {
	case 0:
		s->lines[2] = "Wind Units: MPH";
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

	if (s->line_values[3] == 2) {
		s->lines[0] = "Temp Units: C";
		s->lines[1] = "Press Units: hPa";
		s->lines[2] = "Wind Units: MPH";
		s->line_values[3] = 0;
	}
}

void update_sensor_strings(SCREEN *sensors, SCREEN *settings)
{
	int i;

	for (i = 0; i < sensors->length; i++) {
		
	}
}

char *get_temp_string()
{
}

char *get_pressure_string()
{
}

char *get_wind_string()
{
}
