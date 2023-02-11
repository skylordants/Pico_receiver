#ifndef __AHT20_H__
#define __AHT20_H__

//Temp
//bool aht20_setup ();

//bool aht20_measure(uint32_t *temperature, uint32_t* humidity);

float aht20_calculate_temperature(uint32_t temperature);

float aht20_calculate_humidity(uint32_t humidity);

#endif