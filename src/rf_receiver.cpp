#include <vector>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#include "aht20.h"

#define RF_BIT 250  //The symbol bit length in us
#define RF_SYMBOL 4*RF_BIT  //The symbol length
#define RF_ZERO 1*RF_BIT
#define RF_ONE 3*RF_BIT
#define RF_REP 3
#define RF_REP_PAUSE 2*RF_SYMBOL

#define RF_PREAMPLE 16  //Number of 1 bits before a transmission
#define RF_START 4  //Number of 0 and 1 bit pairs before a packet

#define RECEIVER_PIN 14

#define DEBUG false

uint32_t *aht20_temperature = 0;
uint32_t *aht20_humidity = 0;
int32_t *bmp280_temperature = 0;
uint32_t *bmp280_pressure = 0;

char sensor_packet_header [] = "SENSORS:";

bool rf_receiver_init(uint32_t *aht20_t, uint32_t *aht20_h, int32_t *bmp280_t, uint32_t *bmp280_p) {
	aht20_temperature = aht20_t;
	aht20_humidity = aht20_h;
	bmp280_temperature = bmp280_t;
	bmp280_pressure = bmp280_p;
	gpio_init(RECEIVER_PIN);
	gpio_set_dir(RECEIVER_PIN, GPIO_IN);
	gpio_pull_down(RECEIVER_PIN);
	return true;
}

bool rf_read_bit(uint8_t *bit) {
	bool continuous = true;

	uint32_t begin = time_us_32();
	while (gpio_get(RECEIVER_PIN) == 0) {
		sleep_us(RF_BIT/2);
	}
	uint32_t delay = time_us_32() - begin;
	

	if (delay > 2*RF_SYMBOL) {
		continuous = false;
	}
	
	sleep_us(2*RF_BIT);
	if (gpio_get(RECEIVER_PIN)) {
		*bit = 1;
	}
	else {
		*bit = 0;
	}

	sleep_us(RF_BIT);
	return continuous;
}

bool rf_read_byte(uint8_t *byte) {
	*byte = 0;

	for (int i = 7; i >= 0; i--) {
		uint8_t bit = 0;
		if (rf_read_bit(&bit) == false) {
			return false;
		}
		*byte |= bit <<i;
	}

	return true;
}

uint8_t calculate_error_detection (const char *buffer, uint8_t len) {
	uint8_t res = 0;
	for (int i = 0; i < len; i++) {
		res ^= buffer[i];
	}

	return res;
}

bool rf_read_message() {
	// Preample
	int captured_ones = 0;
	uint8_t last_bit = 0;

	while (captured_ones < 8 || last_bit == 1) {
		if (rf_read_bit(&last_bit) && (last_bit == 1 || captured_ones >= 8)) {
			captured_ones++;
		}
		else {
			captured_ones = 0;
		}
	}

	// Start
	int captured_start = 1;

	while (captured_start < 2*RF_START) {
		uint8_t new_bit = 0;
		if (rf_read_bit(&new_bit) && new_bit != last_bit) {
			last_bit = new_bit;
			captured_start++;
		}
		else {
			return false;
		}
	}

	// Packet length
	uint8_t len = 0;
	if (rf_read_byte(&len) == false || len == 0) {
		return false;
	}

	// Message
	char buffer [257]; // The buffer is 257 bytes long because char buffer makes other stuff easier, the length is specified in 1 byte so 256 values + error checking byte
	for (int i = 0; i < len; i++) {
		uint8_t byte = 0;
		if (rf_read_byte(&byte) == false) {
			return false;
		}
		buffer[i] = byte;
	}

	uint8_t error_code = calculate_error_detection(buffer, len-1);

	if (error_code != buffer[len-1]) {
		if (DEBUG) printf(" Error code doesn't match\n");
		return false;
	}

	if (strncmp(buffer, sensor_packet_header, 8) == 0) { // Ignore the last byte because that is the null byte from string
		if (DEBUG) printf("New sensor data\n");
		memcpy(aht20_temperature, buffer+8, 4);
		memcpy(aht20_humidity, buffer+12, 4);
		memcpy(bmp280_temperature, buffer+16, 4);
		memcpy(bmp280_pressure, buffer+20, 4);

		if (DEBUG) {
			printf("BMP280 - Temperature: %f°C, Pressure %f Pa\n", (float)*bmp280_temperature/100, (float)*bmp280_pressure/256);
			printf("AHT20 - Temperature: %f°C, Humidity %f%%\n", aht20_calculate_temperature(*aht20_temperature), aht20_calculate_humidity(*aht20_humidity));
		}
	}

	return true;
}	