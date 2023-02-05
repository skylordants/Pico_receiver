#ifndef __I2C_H__
#define __I2C_H__

#define SDA_PIN 26
#define SCL_PIN 27

bool i2c_setup();

int i2c_reg_write(const uint addr, const uint8_t reg, uint8_t *buf, const uint8_t nbytes);

int i2c_write(const uint addr, uint8_t *buf, const uint8_t nbytes);

int i2c_reg_read(const uint addr, const uint8_t reg, uint8_t *buf, const uint8_t nbytes);

int i2c_read (const uint addr, uint8_t *buf, const uint8_t nbytes);

#endif