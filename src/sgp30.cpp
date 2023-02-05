// https://www.mouser.com/pdfdocs/Sensirion_Gas_Sensors_SGP30_Datasheet_EN-1148053.pdf

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/timer.h"

#include "i2c.h"
#include "sgp30.h"

#define SGP30_ADDRESS 0x58

#define SGP30_GET_FEATURE_SET 0x202f
#define SGP30_GET_BASELINE 0x2015
#define SGP30_GET_SERIAL_ID 0x3682
#define SGP30_MEASURE_TEST 0x2032
#define SGP30_SET_HUMIDITY 0x2061
#define SGP30_INIT_AIR_QUALITY 0x2003
#define SGP30_MEASURE_AIR_QUALITY 0x2008


#define SGP30_INIT_DELAY 60000000

const uint8_t crc_pol = 0x31;

// Inspiration: http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html
uint8_t calculate_crc(uint8_t data_1, uint8_t data_2) {
	
	uint8_t crc = data_1 ^ 0xff;

	uint8_t data_buffer[2] = {data_2, 0x00};

	for (int b = 0; b < 2; b++) {
		for (int i = 7; i >= 0; i--) {
			bool x = crc & 0x80;

			crc <<= 1;
			crc = (data_buffer[b]&(1<<i)) ? crc|0x01 : crc&0xfe;

			if (x) {
				crc ^= crc_pol;
			}
		}
	}
	return crc;
}

int sgp30_read(uint16_t cmd, uint8_t *buf, const uint8_t nbytes, int delay) {

	uint8_t cmdbuf[2] = { (uint8_t)(cmd>>8), (uint8_t)(cmd&0x00ff) };

	i2c_write(SGP30_ADDRESS, cmdbuf, 2);
	sleep_ms(delay);
	int num_bytes_read = i2c_read(SGP30_ADDRESS, buf, nbytes);

	return num_bytes_read;
}

int sgp30_write(uint16_t cmd, uint8_t *buf, const uint8_t nbytes) {
	int num_bytes_written = 0;
	uint8_t msg[24] = { (uint8_t)(cmd>>8), (uint8_t)(cmd&0x00ff) };

	if (nbytes%2 != 0) {
		printf("Wrong length data");
		return 0;
	}


	for (int i = 0; i < nbytes/2; i++) {
		msg[2+3*i] = buf[2*i];
		msg[3+3*i] = buf[2*i+1];
		msg[4+3*i] = calculate_crc(buf[2*i], buf[2*i+1]);
	}

	// Write data to register(s) over I2C
	num_bytes_written = i2c_write(SGP30_ADDRESS, msg, nbytes/2*3+2);

	return num_bytes_written;
}

uint64_t lasttime = 0;
bool sgp30_init_air_quality() {
	sgp30_write(SGP30_INIT_AIR_QUALITY, NULL, 0);
	sleep_ms(10);
	lasttime = time_us_64();

	return true;
}


bool sgp30_measure_air_quality(uint16_t *co2eq, uint16_t *tvoc) {
	uint8_t data[6];

	int num_bytes_read = sgp30_read(SGP30_MEASURE_AIR_QUALITY, data, 6, 15);

	if (num_bytes_read != 6) {
		lasttime = 0;
		return false;
	}

	uint16_t t_co2eq = ((uint16_t)data[0]<<8)|data[1];
	uint16_t t_tvoc = ((uint16_t)data[3]<<8)|data[4];


	if (t_co2eq == 400 && t_tvoc == 0 && time_us_64() - lasttime > SGP30_INIT_DELAY) {
		sgp30_init_air_quality();
	}
	else {
		*co2eq = t_co2eq;
		*tvoc = t_tvoc;
	}

	return true;
}

bool repeating_measurement_callback (repeating_timer *t) {
	uint16_t **data_pointers = (uint16_t **)(t->user_data);

	sgp30_measure_air_quality(data_pointers[0], data_pointers[1]);
	return true;
}

uint16_t *data_pointers[2];
repeating_timer timer;

bool sgp30_setup(uint16_t *co2eq, uint16_t *tvoc) {
	uint8_t data[9];

	// Start timer
	data_pointers[0] = co2eq;
	data_pointers[1] = tvoc;

	//add_repeating_timer_ms(-1000, repeating_measurement_callback, (void *)data_pointers, &timer);
	return true;
}

bool sgp30_set_humidity(uint16_t humidity) {
	uint8_t data[2] = {(uint8_t)(humidity>>8), (uint8_t)humidity};
	sgp30_write(SGP30_SET_HUMIDITY, data, 2);
	return true;
}

bool sgp30_get_baseline(uint8_t baseline[]) {
	return sgp30_read(SGP30_GET_BASELINE, baseline, 6, 15) == 6;
}