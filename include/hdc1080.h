#ifndef __HDC1080_H__
#define __HDC1080_H__

#include "pico/stdlib.h"

class HDC1080 {
public:
	HDC1080(int temp);
	HDC1080();

	bool measure (uint16_t *temperature, uint16_t *humidity);
	void print_serial_data();

	static float calculate_temperature(uint16_t temperature);
	static float calculate_humidity(uint16_t humidity);
};

#endif