// file sensors.c
#include <stdlib.h>

#include "sensors.h"

/*** Calibration constants ***/

long bmp180_T = 0;
long bmp180_P = 0;

// Some more global variables
//TODO, do away with these
int16_t c_temp = 0; // in 0.1 degree celcius
uint16_t c_humid = 0;
int16_t c_dew_point = 0;
int16_t c_wind_chill = 0;
int16_t c_humidex = 0;


/*** Sensor read functions ***/
// Returns most recently measured temperature, in 0.1 C
int16_t get_temp()
{
	return c_temp;
}


// Returns most recently measured humidity, in 0.1%
uint16_t get_humid()
{
	return c_humid;
}


// Returns most recently measured pressure, in pascals
unsigned long get_pressure()
{
	return bmp180_P;
}

// Returns most recently measured wind speed, in 0.1 mph 
unsigned int get_wind()
{
	return 0;
}

int16_t get_dew_point()
{
	return c_dew_point;
}

int16_t get_humidex()
{
	return c_humidex;
}

int16_t get_wind_chill()
{
	return c_wind_chill;
}
