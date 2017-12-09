#ifndef asdfasdf_sd
#define asdfasdf_sd

#include <stdint.h>

void cs_enable();

void cs_disable();

void sd_init();

uint8_t SPI_write_byte(uint8_t bytes);

void sd_send_command(uint8_t cmd, uint32_t arg, uint8_t crc,
					 uint8_t read_bytes, uint8_t *read_buf);

#endif


