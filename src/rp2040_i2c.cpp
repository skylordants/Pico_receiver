// Inspiration: https://www.digikey.com/en/maker/projects/raspberry-pi-pico-rp2040-i2c-example-with-micropython-and-cc/47d0c922b79342779cdbd4b37b7eb7e2

#include "rp2040_i2c.h"

RP2040_I2C::RP2040_I2C(i2c_inst_t *i2c_instance, uint8_t pSDA, uint8_t pSCL, uint baudrate)
	: _i2c(i2c_instance)
	, _pSDA(pSDA)
	, _pSCL(pSCL)
	, _baudrate(baudrate)
{
	//Initialize I2C port at baudrate
	i2c_init(_i2c, _baudrate);

	// Initialize I2C pins
	gpio_set_function(_pSDA, GPIO_FUNC_I2C);
	gpio_set_function(_pSCL, GPIO_FUNC_I2C);
}

RP2040_I2C::RP2040_I2C() {

}

// Write to the specified register
int RP2040_I2C::reg_write(const uint addr, const uint8_t reg, const uint8_t *buf, const uint8_t nbytes) {
	int num_bytes_written = 0;
	uint8_t msg[1000];

	// Append register address to front of data packet
	msg[0] = reg;
	for (int i = 0; i < nbytes; i++) {
		msg[i + 1] = buf[i];
	}

	// Write data to register(s) over I2C
	num_bytes_written = i2c_write_blocking(_i2c, addr, msg, (nbytes + 1), false);

	return num_bytes_written;
}

int RP2040_I2C::write(const uint addr, const uint8_t *buf, const uint8_t nbytes) {
	int num_bytes_written = 0;

	// Write data to register(s) over I2C
	num_bytes_written = i2c_write_blocking(_i2c, addr, buf, nbytes, false);

	return num_bytes_written;
}

// Read byte(s) from specified register. If nbytes > 1, read from consecutive registers.
int RP2040_I2C::reg_read(const uint addr, const uint8_t reg, uint8_t *buf, const uint8_t nbytes) {
	int num_bytes_read = 0;

	// Check to make sure caller is asking for 1 or more bytes
	if (nbytes < 1) {
		return 0;
	}

	// Read data from register(s) over I2C
	i2c_write_blocking(_i2c, addr, &reg, 1, true);
	num_bytes_read = i2c_read_blocking(_i2c, addr, buf, nbytes, false);

	return num_bytes_read;
}

int RP2040_I2C::read (const uint addr, uint8_t *buf, const uint8_t nbytes) {
	int num_bytes_read = 0;

	// Check to make sure caller is asking for 1 or more bytes
	if (nbytes < 1) {
		return 0;
	}

	// Read data without specifying a register over I2C
	num_bytes_read = i2c_read_blocking(_i2c, addr, buf, nbytes, false);
	return num_bytes_read;
}
