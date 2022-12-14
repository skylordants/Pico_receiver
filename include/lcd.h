#ifndef __LCD_H__
#define __LCD_H__

#define SPI_RX_PIN 0
#define SPI_CS_PIN 1
#define SPI_SCK_PIN 2
#define SPI_TX_PIN 3

bool lcd_setup(spi_inst_t *spi);

void lcd_backlight(bool on);

void lcd_display_on(bool on);

void lcd_DBG_all_points(bool on);

bool lcd_ram_select_address(uint8_t page, uint8_t column);

bool lcd_ram_write(uint8_t *data, uint8_t len);

bool lcd_ram_clear();

bool lcd_write_chars(char *buffer, uint8_t len, uint8_t line, uint8_t pos);

bool lcd_hud_setup();

bool lcd_hud_update_outside_values(float temperature, float humidity, float pressure);

bool lcd_hud_update_inside_values(float temperature, float humidity);

#endif