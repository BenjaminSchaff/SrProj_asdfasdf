#include "defines.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include <stdint.h>

#include "uart.h"
#include "i2c.h"

FILE uart_strm = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

uint16_t temp, humid;
uint16_t c_temp, c_humid; // in dec-celcius and 

/* BMP180 variables */

struct {
	short AC1, AC2, AC3;
	unsigned short AC4, AC5, AC6;
	short B1, B2, MB, MC, MD;
	long B5;
} bmp180_calib;

long UT; // uncalibrated temp
long UP;	// ubcalibrated pressure
long bmp180_T;
/* * */

void i2c_write_test()
{
	i2c_start();
	
	i2c_write_byte((TEMP_SENSE_ADDR<<1) | 0x00);
//	i2c_write_byte((PRES_SENSE_ADDR<<1) | 0x00);
//	i2c_write_byte((0x77<<1) | 0x00); //BMP180
	i2c_write_byte(0xFF);
	i2c_stop();
/*	
	if (val != 0)
		fprintf(&uart_strm, "n%x", val);
	else 
		fprintf(&uart_strm, "y\1");*/
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

long bmp180_calc_true_temp(long u_temp)
{
	long X1, X2, T;

	X1 = (((long)UT - (long)bmp180_calib.AC6)*(long)bmp180_calib.AC5)>>15;
	X2 = ((long)bmp180_calib.MC<<11)/(X1 + bmp180_calib.MD);
	bmp180_calib.B5 = X1 + X2;
	T = (bmp180_calib.B5 + 8)>>4;

	return T;
}

void sensor_init()
{
	bmp180_read_calib();

}



void read_sensors()
{
	// read temp and humidity
	i2c_start();
	i2c_write_byte((TEMP_SENSE_ADDR<<1) | TW_READ);
	temp = (i2c_read_byte(1) << 8);	// temp MSB
	temp |= i2c_read_byte(1);	// temp LSB

	humid = (i2c_read_byte(1) << 8);	// humid MSB
	humid |= i2c_read_byte(0);	// humid LSB
	i2c_stop();

	c_temp = ((uint32_t)(temp) * 1650) / 65536 - 400;
	c_humid = ((uint32_t)(humid) * 1000) / 65536;

	// trigger measurement
	i2c_start();
	i2c_write_byte((TEMP_SENSE_ADDR<<1) | TW_WRITE);
	i2c_write_byte(0x00);
	i2c_stop();
	// wait for measurment (2.5ms to 6.5ms depending on resolution)
	_delay_ms(7);	

	
	/* reading uncalibrated temp and pressure from bmp180 */
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



void init()
{
	DDRA |= (1<<4); // set PA4 to output (LED blink)

	uart_init();
	i2c_init();
	sensor_init();
}


int main() 
{
	init();
	

	while (1) {
		PORTA |= (1<<4);
		_delay_ms(100);
		PORTA &= ~(1<<4);
		_delay_ms(100);

// i2c_write_test();
		read_sensors();

		bmp180_T = bmp180_calc_true_temp(UT);

		//fprintf(&uart_strm, "Temperature: %f C\n", f_temp);
		fprintf(&uart_strm, "T: %d.%dC\n", c_temp/10, c_temp%10);
	//	fprintf(&uart_strm, "H: %d.%d%%\n", c_humid/10, c_humid%10);
		fprintf(&uart_strm, "T2: %ld\n", bmp180_T);
//		fprintf(&uart_strm, "%f%%\n", f_humid);
	}
}
