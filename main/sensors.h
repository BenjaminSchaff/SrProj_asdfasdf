// file sensors.h
#ifndef asdfasdf_sense
#define asdfasdf_sense

#include <stdint.h>

void sensor_init();	// performs sensor initialization

void update_sensors(); // starts measurements on i2c sensors

int16_t get_temp(); // returns temperature in 0.1 degree celcius

uint16_t get_humid(); // returns humidity, in 0.1%

unsigned long get_pressure(); // returns pressure, in pascals

unsigned int get_wind(); // returns wind speed, in 0.1 mph)

uint64_t get_time(); // returns the current times (64bit unit time)

#endif
