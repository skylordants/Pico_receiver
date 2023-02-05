#ifndef __SGP30_H__
#define __SGP30_H__

bool sgp30_setup(uint16_t *co2eq, uint16_t *tvoc);

bool sgp30_measure_air_quality(uint16_t *co2eq, uint16_t *tvoc);

bool sgp30_set_humidity(uint16_t humidity);

bool sgp30_get_baseline(uint8_t baseline[]);

#endif