#ifndef __HDC1080_H__
#define __HDC1080_H__

bool hdc1080_setup ();

bool hdc1080_measure (uint16_t *temperature, uint16_t *humidity);

float hdc1080_calculate_temperature(uint16_t temperature);

float hdc1080_calculate_humidity(uint16_t humidity);

#endif