#include "defines.h"

#include <avr/io.h>
#include "i2c.h" 


#define I2C_SR_START 0x08		// successful start condition
#define I2C_SR_REP_START 0x10	// repeated start condition
#define I2C_SR_SLA_ACK 0x18		// slave address ack
#define I2C_SR_SLA_NO_ACK 0x20		// slave address ack
#define I2C_SR_DATA_ACK 0x28
#define I2C_SR_DATA_NO_ACK 0x30

void i2c_init(void)
{
	
	TWSR = 0x00; // set prescaler to 4;
	/*	SCL frequency = CPU Clock frequency / (16+2(TWBR) * PrescalerValue) */
	/*  "the CPU clock frequency in the Slave must be at least 16 times higher than the SCL frequency" (23.5.2) */
	TWBR = (uint8_t)(F_CPU / I2C_BUS_RATE - 16) / 2;
	// TODO: clock speed seems to be off, as if TWBR is still zero
}



int i2c_start(void)
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);  // begin start transmission

	while ( !(TWCR & (1<<TWINT)) ); 	// wait for transmission to complete

 	// check status register for successful start (or repeated start)
	if (!((TWSR & 0xF8) == I2C_SR_START || (TWSR &0xF8) == I2C_SR_REP_START))
		return (TWSR & 0xF8);  // Error, TODO: deal with this, maybe retransmit?

	return 0;
}

int i2c_write_byte(uint8_t data)
{
	TWDR = data; // put byte into data register

	TWCR = (1<<TWINT) | (1<<TWEN); // begin data transmission

	while ( !(TWCR & (1<<TWINT)) ); // wait for transmission to complete

	if (!((TWSR & 0xF8) == I2C_SR_SLA_ACK || (TWSR &0xF8) == I2C_SR_DATA_ACK)) // check value of status register
		return (TWSR & 0xF8);  // Error, TODO: these should probably be checked separately, (SLA and DATA, that is)

	return 0;
}

uint8_t i2c_read_byte(uint8_t ack)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (ack<<TWEA);	// beging read

	while ( !(TWCR & (1<<TWINT)) );	// wait for read to complete
	//TODO error checking, I guess

	return TWDR;
}

void i2c_stop(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO); // begin stop transmission
}
