/*!
 * @file i2c.h
 *
 * @author Dan Paradis and Ben Schaff
 *
 * Functions for talking to I2C devices using the atmega644pa
 *
 */

#ifndef asdfasdf_i2c
#define asdfasdf_i2c

#define I2C_BUS_RATE 400000UL

/*!
 * Performs initialization to use the I2C bus.
 * Enables TWI and sets clock rate to 400kHz.
 */
void i2c_init(void);

/*!
 * Sends I2C start.
 */
int i2c_start(void);

/*!
 * Sends I2C stop
 */
void i2c_stop(void);

/*!
 * Writes a single byte to the I2C bus
 */
int i2c_write_byte(uint8_t data);

/*!
 * Reads a single byte from the bus.  Sends acknowledgement if ack is 1.
 */
uint8_t i2c_read_byte(uint8_t ack);

#endif
