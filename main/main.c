#include "defines.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include <stdint.h>

#include "uart.h"
#include "i2c.h"
#include "hd44780_settings.h"
#include "hd44780.h"

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
int bmp180_oss = 0;	// pressure oversampling used.  if increased, adjust conversion time
long UT; // uncalibrated temp
long UP;	// ubcalibrated pressure
long bmp180_T;
long bmp180_P;
char buf[20];
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
	bmp180_read_u_temp();
	bmp180_read_u_pres();
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
		bmp180_P = bmp180_calc_true_pres(UP);
		
		lcd_clrscr();
		lcd_home();

		sprintf(buf, "%d", bmp180_T); // convert bmp180 temp to a string
		lcd_puts(buf); // print temperature to first line of screen
		lcd_goto(0x04); // goto next line

		sprintf(buf, "%d", bmp180_P); // convert bmp180 pressure to a string
		lcd_puts(buf); // print pressure to second line of lcd
		_delay_ms(300);

		//fprintf(&uart_strm, "Temperature: %f C\n", f_temp);
//		fprintf(&uart_strm, "T: %d.%dC\n", c_temp/10, c_temp%10);
	//	fprintf(&uart_strm, "H: %d.%d%%\n", c_humid/10, c_humid%10);
//		fprintf(&uart_strm, "T2: %ld\n", bmp180_T);
//		fprintf(&uart_strm, "P: %ld\n", bmp180_P);	
	}
}
