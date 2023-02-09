// Motivation https://github.com/0xJoey/Arduino_93C46

#ifndef __AT93C46_H__
#define __AT93C46_H__

#include "pico/stdlib.h"
#include "SPI.h"

class AT93C46 {
public:
	AT93C46 (uint8_t pCS, uint8_t pSK, uint8_t pDI, uint8_t pDO, spi_inst_t *spi);
	AT93C46 ();
	
	/// @brief Set the memory organization, whether 128 words of 8 bits or 64 words of 16 bits
	/// @param pin The pin connected to EEPROM ORG pin, if less than 0, then assume that it was set in hardware and just to setup software correctly
	/// @param long_mode Whether a word is 16 bits or not
	void set_mode (int pin, bool long_mode);

	// Instruction set
	uint16_t read (uint8_t address);
	void write(uint8_t address, uint16_t data);
	void write_all(uint16_t data);
	void erase(uint8_t address);
	void erase_all();
	void ew_enable();
	void ew_disable();

	// Additional functionality
	void read_words (uint8_t address, uint16_t *data, uint8_t len);
	void write_words (uint8_t address, uint16_t *data, uint8_t len);

private:
	bool _ew;
	bool _long_mode;

	uint8_t _pCS;
	uint8_t _pSK;
	uint8_t _pDI;
	uint8_t _pDO;
	int8_t _pORG;

	spi_inst_t *_spi;
};

#endif