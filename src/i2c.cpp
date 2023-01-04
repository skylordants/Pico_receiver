// Inspiration: https://www.digikey.com/en/maker/projects/raspberry-pi-pico-rp2040-i2c-example-with-micropython-and-cc/47d0c922b79342779cdbd4b37b7eb7e2

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define SDA_PIN 26
#define SCL_PIN 27

i2c_inst_t *i2c = i2c1;

bool i2c_setup() {
	//Initialize I2C port at 400 kHz
	i2c_init(i2c, 400 * 1000);

	// Initialize I2C pins
	gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
	gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
	return true;
}

// Write to the specified register
int i2c_reg_write(const uint addr, const uint8_t reg, uint8_t *buf, const uint8_t nbytes) {
	int num_bytes_read = 0;
	uint8_t msg[nbytes + 1];

	// Append register address to front of data packet
	msg[0] = reg;
	for (int i = 0; i < nbytes; i++) {
		msg[i + 1] = buf[i];
	}

	// Write data to register(s) over I2C
	num_bytes_read = i2c_write_blocking(i2c, addr, msg, (nbytes + 1), false);

	return num_bytes_read;
}

// Read byte(s) from specified register. If nbytes > 1, read from consecutive
// registers.
int i2c_reg_read(const uint addr, const uint8_t reg, uint8_t *buf, const uint8_t nbytes) {

	int num_bytes_read = 0;

	// Check to make sure caller is asking for 1 or more bytes
	if (nbytes < 1) {
		return 0;
	}

	// Read data from register(s) over I2C
	i2c_write_blocking(i2c, addr, &reg, 1, true);
	num_bytes_read = i2c_read_blocking(i2c, addr, buf, nbytes, false);

	return num_bytes_read;
}

int i2c_read (const uint addr, uint8_t *buf, const uint8_t nbytes) {
	int num_bytes_read = 0;
	
	if (nbytes < 1) {
		return 0;
	}

	num_bytes_read = i2c_read_blocking(i2c, addr, buf, nbytes, false);
	return num_bytes_read;
}
