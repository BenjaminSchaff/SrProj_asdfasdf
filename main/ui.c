#include "defines.h"

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>

#include "ui.h"
#include "hd44780_settings.h"
#include "hd44780.h"
#include "sensors.h"

void update_screen_state(int button, int current_screen_index, SCREEN *current_screen)
{
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
	lcd_puts((current_screen->lines)[current_screen->screen_index]);
	lcd_goto(0x40);
	lcd_puts((current_screen->lines)[current_screen->screen_index + 1]);
	lcd_goto(0x14);
	lcd_puts((current_screen->lines)[current_screen->screen_index + 2]);
	lcd_goto(0x54);
	lcd_puts((current_screen->lines)[current_screen->screen_index + 3]);

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
	switch (s->line_values[0]) { // check what value temp is
	case 0:
		sprintf(s->lines[0], "Temp Units: C"); // choose C or F depending on it
		break;
	case 1:
		sprintf(s->lines[0], "Temp Units: F");
		break;
	}

	switch (s->line_values[1]) { // check what value Pressure should be
	case 0:
		sprintf(s->lines[1], "Press Units: hPa"); // choose hPa, atm, or bars
		break;
	case 1:
		sprintf(s->lines[1], "Press Units: psi");
		break;
	case 2:
		sprintf(s->lines[1], "Press Units: bars");
		break;
	}

	switch (s->line_values[2]) { // check what value wind should be
	case 0:
		sprintf(s->lines[2], "Wind Units: MPH"); // pick mph, kph, m/s, or ft/s depending on the value
		break;
	case 1:
		sprintf(s->lines[2], "Wind Units: KPH");
		break;
	case 2:
		sprintf(s->lines[2], "Wind Units: ft/s");
		break;
	case 3:
		sprintf(s->lines[2], "Wind Units: m/s");
		break;
	}

	if (s->line_values[3] == 2) { // reset everything to zero
		sprintf(s->lines[0], "Temp Units: C");
		sprintf(s->lines[1], "Press Units: hPa");
		sprintf(s->lines[2], "Wind Units: MPH");
		s->line_values[0] = 0;
		s->line_values[1] = 0;
		s->line_values[2] = 0;
		s->line_values[3] = 0;
	}
}

void update_sensor_strings(SCREEN *sensors, SCREEN *settings, char ret[21])
{
	sprintf(sensors->lines[0], get_temp_string(settings, ret));
	sprintf(sensors->lines[1], get_pressure_string(settings, ret));
	sprintf(sensors->lines[2], get_wind_string(settings, ret));
	sprintf(sensors->lines[3], get_humidity_string(ret));
	sprintf(sensors->lines[4], get_wind_chill_string(ret));
	sprintf(sensors->lines[5], get_dew_point_string(ret));
	sprintf(sensors->lines[6], get_humidex_string(ret));
}

/* Changes units for temperature in accordance to what the settings say        */
/* Returns the string that should be saved in the sensor screen's lines array */
char *get_temp_string(SCREEN *settings, char ret[21])
{
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
char *get_pressure_string(SCREEN *settings, char ret[21])
{
	unsigned long pressure = get_pressure();

	if (settings->line_values[1] == 0) { // if setting is 0 use hPa as unit
		sprintf(ret, "Pres: %lu.%lu hPa", pressure/100, pressure%100); // print to ret, the pressure in hPa
	} else if (settings->line_values[1] == 1) {
		pressure = (pressure * 10) / 6894 ; // convert to psi
		sprintf(ret, "Pres: %lu.%lu psi", pressure/10, pressure%10);
	} else { // convert to bars
		pressure = (pressure * 100) / 10000; // the * 100 is to preserve sig figs
		if (pressure < 1000)
			sprintf(ret, "Pres: 0.%lu bars", pressure);
		else
			sprintf(ret, "Pres: 1.%lu bars", (pressure/10)%10);
	}

	return ret;
}

/* Changes units for wind speed in accordance to what the settings say        */
/* Returns the string that should be saved in the sensor screen's lines array */
char *get_wind_string(SCREEN *settings, char ret[21])
{
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

	return ret;
}

char *get_humidity_string(char ret[21])
{
	uint16_t humid = get_humid();
	sprintf(ret, "Humidity: %u%% RH", humid);

	return ret;
}

char *get_wind_chill_string(char ret[21])
{
	int16_t wind_chill = get_wind_chill();
	sprintf(ret, "Wind Chill: %d C", wind_chill);

	return ret;
}

char *get_dew_point_string(char ret[21])
{
    int16_t dew_point = get_dew_point();
    sprintf(ret, "Dew Point: %d C", dew_point);

    return ret;
}

char *get_humidex_string(char ret[21])
{
    int16_t humidex = get_humidex();
    sprintf(ret, "Humidex: %d C", humidex);

    return ret;
}

void ui_init(SCREEN ui[3])
{
	int i;

	SCREEN *main_menu = &ui[0];
	SCREEN *sensor_readouts = &ui[1];
	SCREEN *settings = &ui[2];

	// initialize main menu to correct data
	sprintf(main_menu->lines[0], "Sensors");
	sprintf(main_menu->lines[1], "Saved Data");
	sprintf(main_menu->lines[2], "Settings");
	sprintf(main_menu->lines[3],  " ");

	main_menu->length = 4;
	main_menu->screen_index = 0;
	main_menu->curser_index = 0;

	for (i = 0; i < main_menu->length; i++) {
		main_menu->line_values[i] = 0;
		main_menu->max_values[i] = 0;
	}

	// initialize sensor screen to right data
	sprintf(sensor_readouts->lines[0], "Temp: 0 C");
	sprintf(sensor_readouts->lines[1], "Pres: 0 hPa");
	sprintf(sensor_readouts->lines[2], "Wind: 0 MPH");
	sprintf(sensor_readouts->lines[3], "Humidity: 0%% RH");
	sprintf(sensor_readouts->lines[4], "Wind Chill: 0 C");
	sprintf(sensor_readouts->lines[5], "Dew Point 0");
	sprintf(sensor_readouts->lines[6], "Humidex: 0");

	sensor_readouts->length = 7;
	sensor_readouts->screen_index = 0;
	sensor_readouts->curser_index = 0;

	for (i = 0; i < sensor_readouts->length; i++) {
		sensor_readouts->line_values[i] = i;
		sensor_readouts->max_values[i] = 6;
	}

	// initialize setting screen to correct data
	sprintf(settings->lines[0], "Temp Units: C");
	sprintf(settings->lines[1], "Press Units: hPa");
	sprintf(settings->lines[2], "Wind Units: MPH");
	sprintf(settings->lines[3], "Factory Defaults");

	settings->length = 4;
	settings->screen_index = 0;
	settings->curser_index = 0;

	for (i = 0; i < settings->length; i++)
		settings->line_values[i] = 0;

	settings->max_values[0] = 1;
	settings->max_values[1] = 2;
	settings->max_values[2] = 3;
	settings->max_values[3] = 2;
}
