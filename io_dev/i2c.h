#ifndef asdfasdf_i2c
#define asdfasdf_i2c

#define TEMP_SENSE_ADDR 0x40
#define PRES_SENSE_ADDR 0x5C

#define I2C_BUS_RATE 400000UL

void i2c_init(void);

int i2c_start(void);
void i2c_stop(void);
int i2c_write_byte(uint8_t data);
uint8_t i2c_read_byte(uint8_t ack);

#endif
