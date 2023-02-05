// https://www.ti.com/lit/ds/symlink/hdc1080.pdf?ts=1672754459455

#include <stdio.h>

#include "pico/stdlib.h"

#include "hdc1080.h"
#include "i2c.h"

#define HDC1080_ADDRESS 0x40

#define HDC1080_REG_TEMP 0x00
#define HDC1080_REG_HUMI 0x01
#define HDC1080_REG_CONF 0x02
#define HDC1080_REG_SERIAL_ID 0xFB

HDC1080::HDC1080 (int temp) {
	uint8_t data[1] = {0b00010000};
	i2c_reg_write(HDC1080_ADDRESS, HDC1080_REG_CONF, data, 1);
}

HDC1080::HDC1080 () {
	
}

bool HDC1080::measure (uint16_t *temperature, uint16_t *humidity) {
	uint8_t data[4];
	i2c_reg_write(HDC1080_ADDRESS, HDC1080_REG_TEMP, data, 0);

	int read;

	sleep_ms(14);
	do {
		read = i2c_read(HDC1080_ADDRESS, data, 4);

		if (read == PICO_ERROR_GENERIC) {
			return false;
		}
	} while (read != 4);

	*temperature = (data[0]<<8)|data[1];
	*humidity = (data[2]<<8)|data[3];

	return true;
}

void HDC1080::print_serial_data() {
	uint8_t data[10];
	for (int i = 0; i < 5; i++) {
		i2c_reg_read(HDC1080_ADDRESS, HDC1080_REG_SERIAL_ID+i, data+2*i, 2);
	}

	for (int i = 0; i < 10; i++) {
		printf("%02x", data[i]);
	}
	printf("\n");
}

float HDC1080::calculate_temperature(uint16_t temperature) {
	return (float)(((double)temperature/(1<<16))*165-40);
}

float HDC1080::calculate_humidity(uint16_t humidity) {
	return (float)(((double)humidity/(1<<16))*100);
}