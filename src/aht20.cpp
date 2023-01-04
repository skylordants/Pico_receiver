// Inspiration: https://github.com/DFRobot/DFRobot_AHT20/blob/master/DFRobot_AHT20.cpp
// https://files.seeedstudio.com/wiki/Grove-AHT20_I2C_Industrial_Grade_Temperature_and_Humidity_Sensor/AHT20-datasheet-2020-4-16.pdf

#include "pico/stdlib.h"

#include "i2c.h"

#define AHT20_ADDRESS 0x38
#define AHT20_REG_STATUS 0x71
#define AHT20_INIT 0xBE
#define AHT20_INIT_P1 0x08
#define AHT20_INIT_P2 0x00
#define AHT20_TRIG 0xAC
#define AHT20_TRIG_P1 0x33
#define AHT20_TRIG_P2 0x00

//TODO: Check if working
bool aht20_setup () {
	uint8_t data[2];
	i2c_reg_read(AHT20_ADDRESS, AHT20_REG_STATUS, data, 1);

	// Device not calibrated
	if ((data[0]&(1<<3)) == 0) {
		data[0] = AHT20_INIT_P1;
		data[1] = AHT20_INIT_P2;

		i2c_reg_write(AHT20_ADDRESS, AHT20_INIT, data, 2);
	}

	return true;
}

//TODO: Check validity
bool aht20_measure(uint32_t *temperature, uint32_t* humidity) {
	uint8_t data[6] = {AHT20_TRIG_P1, AHT20_TRIG_P2};
	
	i2c_reg_write(AHT20_ADDRESS, AHT20_TRIG, data, 2);
	sleep_ms(100);
	do {
		i2c_reg_read(AHT20_ADDRESS, AHT20_REG_STATUS, data, 1);

		sleep_ms(20);
	} while ((data[0]&1) == 1);
	

	i2c_read(AHT20_ADDRESS, data, 6);

	*temperature = (int32_t)(data[3]&0x0F)<<16 | (int32_t)data[4]<<8 | data[5];
	*humidity = (int32_t)data[1]<<12 | (int32_t)data[2]<<4 | data[3]>>4;

	return true;
}

float aht20_calculate_temperature(uint32_t temperature) {
	return (float)(((double)temperature/(1<<20))*200-50);
}

float aht20_calculate_humidity(uint32_t humidity) {
	return (float)((double)humidity/(1<<20)*100);
}