// file sensors.c

#include "defines.h"

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h>

#include "sensors.h"
#include "i2c.h"

#define TEMP_SENSE_ADDR 0x40
#define PRES_SENSE_ADDR 0x5C
#define BMP180_ADDR 0x77



/*** Calibration constants ***/

int16_t t_offset = -25; // temp calibration (constant offset)
int wind_freq_divider = 52; // wind speed calibration (constant multiplier)



/*** Anememometer Variables ***/
volatile uint16_t prev_timer_capture = 0;
volatile uint8_t wind_index = 0;
volatile uint32_t wind_periods[NUM_WIND_SAMP];
volatile uint32_t n_overflows = 0;

uint32_t avg_wind_freq;


/*** BMP180 variables ***/
struct {
	short AC1, AC2, AC3;
	unsigned short AC4, AC5, AC6;
	short B1, B2, MB, MC, MD;
	long B5;
} bmp180_calib; // factory-set calibration constants read from sensor at start

int bmp180_oss = 0;	// pressure oversampling used.  if increased, adjust conversion time
long UT; // uncalibrated temp
long UP;	// ubcalibrated pressure
long bmp180_T;
long bmp180_P;
int p_offset = 1100;


/*****************************/

// Some more global variables
//TODO, do away with these
int16_t c_temp; // in 0.1 degree celcius
uint16_t c_humid;
int16_t c_dew_point;
int16_t c_wind_chill;
int16_t c_humidex;


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
	return (avg_wind_freq*10)/wind_freq_divider;
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

uint64_t get_time()
{
	static uint64_t time = 0;
	return time++;
}

// Anemometer input capture counter
ISR(TIMER1_CAPT_vect)
{
	uint16_t tmp = ICR1; // get current timer value
	wind_periods[wind_index] = ((n_overflows << 16) + tmp) - prev_timer_capture;
	n_overflows = 0;
	
	wind_index++;
	if (wind_index == NUM_WIND_SAMP)
		wind_index = 0;

	prev_timer_capture = tmp;
}

// Counting overflows on input capture timer
ISR(TIMER1_OVF_vect)
{
	// TODO handle extremely low rpm elegantly
	n_overflows++;
}



void bmp180_read_calib(void) 
{
	i2c_start();
	i2c_write_byte(BMP180_ADDR<<1 | TW_WRITE);	// device addr
	i2c_write_byte(0xAA);	// addr of first byte (AC1 msb)
	i2c_start();	// restart
	i2c_write_byte(BMP180_ADDR<<1 | TW_READ);
	
	bmp180_calib.AC1 = (i2c_read_byte(1) << 8) + i2c_read_byte(1);
	bmp180_calib.AC2 = (i2c_read_byte(1) << 8) + i2c_read_byte(1);
	bmp180_calib.AC3 = (i2c_read_byte(1) << 8) + i2c_read_byte(1);
	bmp180_calib.AC4 = (i2c_read_byte(1) << 8) + i2c_read_byte(1);
	bmp180_calib.AC5 = (i2c_read_byte(1) << 8) + i2c_read_byte(1);
	bmp180_calib.AC6 = (i2c_read_byte(1) << 8) + i2c_read_byte(1);
	bmp180_calib.B1 = (i2c_read_byte(1) << 8) + i2c_read_byte(1);
	bmp180_calib.B2 = (i2c_read_byte(1) << 8) + i2c_read_byte(1);
	bmp180_calib.MB = (i2c_read_byte(1) << 8) + i2c_read_byte(1);
	bmp180_calib.MC = (i2c_read_byte(1) << 8) + i2c_read_byte(1);
	bmp180_calib.MD = (i2c_read_byte(1) << 8) + i2c_read_byte(0);

	i2c_stop();
}

void bmp180_read_u_temp(void)
{
	// trigger temp start
	i2c_start();
	i2c_write_byte(BMP180_ADDR<<1 | TW_WRITE); // device address
	i2c_write_byte(0xF4);	// register address
	i2c_write_byte(0x2E);	// register data
	i2c_stop();
	
	_delay_ms(5); // wait for temp conversion
	// read ut
	i2c_start();
	i2c_write_byte(BMP180_ADDR<<1 | TW_WRITE); // device address
	i2c_write_byte(0xF6);	// read addr (temp msb)
	i2c_start();	// restart
	i2c_write_byte(BMP180_ADDR<<1 | TW_READ); // begin read
	UT = (i2c_read_byte(1) << 8) + i2c_read_byte(0);
	i2c_stop();
}

//TODO break this into separate start and read functions, and read returns UP
void bmp180_read_u_pres(void)
{
	// trigger pressure start by writing 0x34+(oss<<6) to reg 0xF4 and waiting
	i2c_start();
	i2c_write_byte(BMP180_ADDR<<1 | TW_WRITE); 
	i2c_write_byte(0xF4);	// register addr
	i2c_write_byte(0x34+(bmp180_oss<<6));	// register data
	i2c_stop();

	_delay_ms(5);	// 5ms conversion time for 0 oversampling
	
	// read UP
	i2c_start();
	i2c_write_byte(BMP180_ADDR<<1 | TW_WRITE); // sending read addr
	i2c_write_byte(0xF6);	// read addr (pressure MSB)
	i2c_start();	// restart
	i2c_write_byte(BMP180_ADDR<<1 | TW_READ); //  begin read
	UP = (((long)i2c_read_byte(1) << 16) + ((long)i2c_read_byte(1) << 8) + i2c_read_byte(0)) >> (8-bmp180_oss);
	i2c_stop();
}


long bmp180_calc_true_temp(long u_temp)
{
	long X1, X2, T;

	X1 = (((long)UT - (long)bmp180_calib.AC6)*(long)bmp180_calib.AC5)>>15;
	X2 = ((long)bmp180_calib.MC<<11)/(X1 + bmp180_calib.MD);
	bmp180_calib.B5 = X1 + X2;
	T = (bmp180_calib.B5 + 8)>>4;

	return T;
}


long bmp180_calc_true_pres(long u_pres)
{
	// from datasheet.
	long X1, X2, X3, B3, B6, P;
	unsigned long B4, B7;
	B6 = bmp180_calib.B5 - 4000;
	X1 = ((long)bmp180_calib.B2 * (B6 * B6)>>12)>>11;
	X2 = ((long)bmp180_calib.AC2 * B6)>>11;
	X3 = X1 + X2;
	B3 = (((((long)bmp180_calib.AC1)*4 + X3)<<bmp180_oss) + 2)>>2;
	X1 = ((long)bmp180_calib.AC3 * B6)>>13;
	X2 = ((long)bmp180_calib.B1 * ((B6 * B6)>>12))>>16;
	X3 = ((X1 + X2) + 2)>>2;
	B4 = ((long)bmp180_calib.AC4 * (unsigned long)(X3 + 32768))>>15;
	B7 = ((unsigned long)u_pres - B3) * (50000>>bmp180_oss);
	if (B7 < 0x80000000UL)	P = (B7 * 2)/B4;
	else P = (B7/B4) * 2;

	X1 = (P>>8) * (P>>8);
	X1 = (X1 * 3038) >> 16;
	X2 = (-7357 * P)>>16;

	P += (X1 + X2 + 3791) >> 4;

	return P;
}


void sensor_init()
{
	bmp180_read_calib();
	SREG |= 0x80; // enable global interrupt bit
	TCCR1A = 0x00; // don't need any of these
	TCCR1B = 0x42; // turns off noise cancellation, turns on rising edge detection, positve edge, prescalar = 8
	TIMSK1 |= (1 << ICIE1); // input capture interrupt enable
	TIMSK1 |= (1 << TOIE1); // overflow interrupt enable
}

void update_sensors()
{
	int i;
	uint16_t temp, humid;
	

	// read temp and humidity
	i2c_start();
	i2c_write_byte((TEMP_SENSE_ADDR<<1) | TW_READ);
	temp = (i2c_read_byte(1) << 8);	// temp MSB
	temp |= i2c_read_byte(1);	// temp LSB

	humid = (i2c_read_byte(1) << 8);	// humid MSB
	humid |= i2c_read_byte(0);	// humid LSB
	i2c_stop();

	c_temp = (((int32_t)(temp) * 1650) / 65536 - 400) + t_offset;
	c_humid = ((uint32_t)(humid) * 1000) / 65536;
	c_dew_point = c_temp - (100 - c_humid) / 5; // calculate dewpoint
	c_wind_chill = c_temp;
	c_humidex = c_temp;

	// trigger measurement
	i2c_start();
	i2c_write_byte((TEMP_SENSE_ADDR<<1) | TW_WRITE);
	i2c_write_byte(0x00);
	i2c_stop();
	// wait for measurment (2.5ms to 6.5ms depending on resolution)
	_delay_ms(7);	

	
	/* reading uncalibrated temp and pressure from bmp180 */
	bmp180_read_u_temp();
	bmp180_read_u_pres();

	/* Calculate average period */
	avg_wind_freq = 0;
	for (i = 0; i < NUM_WIND_SAMP; i++)
		avg_wind_freq += wind_periods[i];

	avg_wind_freq /= NUM_WIND_SAMP; // average period

	// convert to frequency from period
	avg_wind_freq = 1000000/avg_wind_freq;


	bmp180_T = bmp180_calc_true_temp(UT);
	bmp180_P = bmp180_calc_true_pres(UP) + p_offset;	
}
