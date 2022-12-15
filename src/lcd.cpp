// Inspiration: https://github.com/adafruit/ST7565-LCD
// https://www.lcd-module.de/eng/pdf/zubehoer/st7565r.pdf

#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#include "spi.h"
#include "lcd.h"
#include "glcdfont.h"

#define BACKLIGHT_PIN 4
#define CD_PIN 5
#define RST_PIN 6

spi_inst_t *spi;

uint8_t ram_page_address = 0x00;
uint8_t ram_column_address = 0x00;

bool send_command(uint8_t *buffer, uint8_t len, bool write) {
	gpio_put(CD_PIN, write);

	gpio_put(SPI_CS_PIN, 0);
	spi_write_blocking(spi, buffer, len);
	gpio_put(SPI_CS_PIN, 1);
	return true;
}

bool lcd_setup(spi_inst_t *tspi) {
	gpio_init(SPI_CS_PIN);
  	gpio_set_dir(SPI_CS_PIN, GPIO_OUT);
	gpio_put(SPI_CS_PIN, 1);
	spi = tspi;

	// Initialize SPI port at 1 MHz
	spi_init(spi, 20*1000 * 1000);

	// Set SPI format
	spi_set_format( spi0,   // SPI instance
									8,      // Number of bits per transfer
									SPI_CPOL_0,      // Polarity (CPOL)
									SPI_CPHA_1,      // Phase (CPHA)
									SPI_MSB_FIRST);

	// Initialize SPI pins
	gpio_set_function(SPI_SCK_PIN, GPIO_FUNC_SPI);
	gpio_set_function(SPI_TX_PIN, GPIO_FUNC_SPI);
	gpio_set_function(SPI_RX_PIN, GPIO_FUNC_SPI);


	// CD
	gpio_init(CD_PIN);
	gpio_set_dir(CD_PIN, GPIO_OUT);

	// Backlight
	gpio_init(BACKLIGHT_PIN);
	gpio_set_dir(BACKLIGHT_PIN, GPIO_OUT);

	// Reset
	gpio_init(RST_PIN);
	gpio_set_dir(RST_PIN, GPIO_OUT);
	gpio_put(SPI_CS_PIN, 0);
	gpio_put(RST_PIN, 0);
	sleep_ms(500);
	gpio_put(RST_PIN, 1);
	gpio_put(SPI_CS_PIN, 1);


	uint8_t data[1];

	// LCD bias select
	data[0] = 0xA3;
	send_command(data, 1, 0);

	// ADC select
	data[0] = 0xA0;
	send_command(data, 1, 0);

	// COM select
	data[0] = 0xC0;
	send_command(data, 1, 0);

	// Start line select
	data[0] = 0x40;
	send_command(data, 1, 0);

	// Voltage converter
	data[0] = 0x2C;
	send_command(data, 1, 0);

	sleep_ms(50);

	// Voltage regulator
	data[0] = 0x2E;
	send_command(data, 1, 0);

	sleep_ms(50);

	// Voltage follower
	data[0] = 0x2F;
	send_command(data, 1, 0);

	sleep_ms(10);

	// LCD operating voltage
	data[0] = 0x26;
	send_command(data, 1, 0);

	lcd_ram_clear();

	lcd_display_on(true);

	return true;
}

void lcd_backlight(bool on) {
	gpio_put(BACKLIGHT_PIN, on);
}

void lcd_display_on(bool on) {
	uint8_t data[1] = {(uint8_t)(0xae | on)};
	send_command(data, 1, 0);
}

void lcd_DBG_all_points(bool on) {
	uint8_t data[1] = {(uint8_t)(0xa4 | on)};
	printf("%x\n", data[0]);
	send_command(data, 1, 0);
}

bool lcd_ram_select_address(uint8_t page, uint8_t column) {
	if (page > 7) {
		printf("Wrong Page Address\n");
		return false;
	}

	if (column > 127) {
		printf("Wrong Column Address\n");
		return false;
	}

	uint8_t data[3] = {(uint8_t)(0xB0 | page), (uint8_t)( 0x10 | (column>>4)),(uint8_t)(0x00 | (column&0x0F))};
	return send_command(data, 3, 0);
}

bool lcd_ram_write(uint8_t *data, uint8_t len) {
	return send_command(data, len, 1);
}

bool lcd_ram_clear() {
	uint8_t buf [128];
	for (int i = 0; i < 128; i++) {
		buf[i] = 0x00;
	}
	for (int page = 0; page < 8; page++) {

		lcd_ram_select_address(page, 0);
		lcd_ram_write(buf, 128);
	}

	return true;
}

bool lcd_write_chars(char *buffer, uint8_t len, uint8_t line, uint8_t pos) {
	lcd_ram_select_address(line, pos*5);

	for (int i = 0; i < len; i++) {
		uint8_t data[5];
		for (int j = 0; j < 5; j++) {
			data[j] = font[(uint8_t)buffer[i]*5 + j];
		}
		lcd_ram_write(data, 5);
	}
	return true;
}

bool lcd_hud_setup() {
	char buf[21] = "hPaC%OUTSIDE:INSIDE:";
	lcd_write_chars(buf, 3, 5, 20);
	lcd_write_chars(buf+3, 1, 6, 9);
	lcd_write_chars(buf+4, 1, 6, 20);
	lcd_write_chars(buf+5, 8, 7, 0);

	lcd_ram_select_address(6, 8*5);
	lcd_ram_write(deg, 5);

	lcd_write_chars(buf, 3, 1, 20);
	lcd_write_chars(buf+3, 1, 2, 9);
	lcd_write_chars(buf+4, 1, 2, 20);
	lcd_write_chars(buf+13, 7, 3, 0);

	lcd_ram_select_address(2, 8*5);
	lcd_ram_write(deg, 5);

	return true;
}

bool lcd_hud_update_outside_values(float temperature, float humidity, float pressure) {
	char t[10], h[10], p[10];

	snprintf(t, sizeof t, "%f", temperature);
	snprintf(h, sizeof h, "%f", humidity);
	snprintf(p, sizeof p, "%f", pressure);

	lcd_write_chars(t, 5, 6, 3);
	lcd_write_chars(h, 5, 6, 14);
	lcd_write_chars(p, 5, 5, 14);
	return true;
}

bool lcd_hud_update_inside_values(float temperature, float humidity, float pressure) {
	char t[10], h[10], p[10];

	snprintf(t, sizeof t, "%f", temperature);
	snprintf(h, sizeof h, "%f", humidity);
	snprintf(p, sizeof p, "%f", pressure);

	lcd_write_chars(t, 5, 2, 3);
	lcd_write_chars(h, 5, 2, 14);
	lcd_write_chars(p, 5, 1, 14);
	return true;
}