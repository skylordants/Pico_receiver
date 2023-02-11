#ifndef __HDC1080_H__
#define __HDC1080_H__

#include "pico/stdlib.h"
#include "rp2040_i2c.h"

class HDC1080 {
public:
	HDC1080(RP2040_I2C *i2c);
	HDC1080();

	bool measure ();
	void print_serial_data();

	static float calculate_temperature(uint16_t temperature);
	static float calculate_humidity(uint16_t humidity);

	float calculate_current_temperature();
	float calculate_current_humidity();

	uint16_t temperature;
	uint16_t humidity;
private:
	RP2040_I2C *_i2c;
};

#endif