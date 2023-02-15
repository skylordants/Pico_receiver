#ifndef __AHT20_H__
#define __AHT20_H__

#include "rp2040_i2c.h"

class AHT20 {
public:
	AHT20(RP2040_I2C *i2c);
	AHT20();

	bool measure();

	static float calculate_temperature(uint32_t temperature);
	static float calculate_humidity(uint32_t humidity);

	float calculate_current_temperature();
	float calculate_current_humidity();

	uint32_t temperature;
	uint32_t humidity;
private:
	RP2040_I2C *_i2c;
};

#endif