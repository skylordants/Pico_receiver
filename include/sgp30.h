#ifndef __SGP30_H__
#define __SGP30_H__

#include "pico/stdlib.h"

class SGP30 {
public:
	SGP30(int temp);
	SGP30();

	/// @brief Measure air quality. Call at 1 Hz frequency for best result.
	bool measure_air_quality(uint16_t *co2eq, uint16_t *tvoc);
	
	bool get_baseline(uint8_t baseline[]);

	bool set_baseline(uint8_t baseline[]);
	bool set_humidity(uint16_t humidity);
private:
	int read(uint16_t cmd, uint8_t *buf, const uint8_t nbytes, int delay);
	int write(uint16_t cmd, uint8_t *buf, const uint8_t nbytes);
	bool init_air_quality();

	uint64_t lasttime;


	static const uint8_t crc_pol;
	static uint8_t calculate_crc(uint8_t data_1, uint8_t data_2);
};

#endif