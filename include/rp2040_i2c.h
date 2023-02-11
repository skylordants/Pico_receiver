#ifndef __I2C_H__
#define __I2C_H__

#include "hardware/i2c.h"
#include "pico/stdlib.h"

class RP2040_I2C {
public:
	RP2040_I2C (i2c_inst_t *i2c_instance, uint8_t pSDA, uint8_t pSCL, uint baudrate);
	RP2040_I2C ();

	int reg_write(const uint addr, const uint8_t reg, const uint8_t *buf, const uint8_t nbytes);
	int write(const uint addr, const uint8_t *buf, const uint8_t nbytes);
	int reg_read(const uint addr, const uint8_t reg, uint8_t *buf, const uint8_t nbytes);
	int read (const uint addr, uint8_t *buf, const uint8_t nbytes);

private:
	i2c_inst_t *_i2c;
	uint8_t _pSDA;
	uint8_t _pSCL;
	uint _baudrate;
};

#endif