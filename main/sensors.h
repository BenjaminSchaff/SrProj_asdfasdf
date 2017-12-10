// file sensors.h
#ifndef asdfasdf_sense
#define asdfasdf_sense

#include <stdint.h>

/*!
 * Performs sensor initialization, requires i2c to be initialized.
 */
void sensor_init();	

/*!
 * Starts measurements on i2c sensors and updates saved measurements.
 */
void update_sensors();

/*!
 * Returns most recently measured temperature in 0.1 degree celcius
 */
int16_t get_temp();

/*!
 * Returns most recently measured humidity, in 0.1% RH
 */
uint16_t get_humid();

/*!
 * Returns most recently measured pressure, in pascals
 */
unsigned long get_pressure();

/*!
 * Returns most recently measured wind speed, in 0.1 mph
 */
unsigned int get_wind();

/*!
 * Returns the current time (64bit unit time)
 */
uint64_t get_time(); 

/*!
 * Returns calculated wind chill in 0.1 degrees celcius
 */
int16_t get_wind_chill();

/*!
 * Returns calculated dew point in 0.1 degress celcius
 */
int16_t get_dew_point();

/*!
 * Returns calculated humidex in 0.1 degrees celcius
 */
int16_t get_humidex();

#endif
