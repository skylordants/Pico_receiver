#ifndef __BMP280_H__
#define __BMP280_H__

bool bmp280_setup();

bool bmp280_measure(int32_t *temperature, uint32_t *pressure);

#endif