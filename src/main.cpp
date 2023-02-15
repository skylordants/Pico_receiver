// Inspired by https://github.com/wagiminator/ATtiny13-TinyRemoteRF/blob/main/software/remote_4_buttons/RF_Remote_4B.ino

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/spi.h"
#include "pico/time.h"

#include "pins.h"

#include "receiver_lcd.h"
#include "rf_receiver.h"
#include "aht20.h"
#include "hdc1080.h"
#include "rp2040_i2c.h"
#include "sgp30.h"
#include "at93c46.h"

#define SGP30_MEASUREMENT_DELAY 1000000
#define HUMIDITY_UPDATE 5000000
#define DISPLAY_RESET 10*60*1000000

#define LCD_BRIGHT_TIME 10*1000000
#define BUTTON_BACKLIGHT_PERMAON_TIME 3*1000000

uint32_t ext_aht20_t = 0;
uint32_t ext_aht20_h = 0;
int32_t ext_bmp280_t = 0;
uint32_t ext_bmp280_p = 0;


RP2040_I2C i2c;
SGP30 sgp30;
HDC1080 hdc1080;
AT93C46 eeprom;
ReceiverLCD lcd;

uint64_t lasthumidity = 0;

uint64_t button_brightness_last_accepted = 0;
uint64_t lcd_backlight_start = 0;
bool lcd_backlight_on = false;
bool lcd_backlight_permanent = false;

void button_callback(uint gpio, uint32_t event_mask) {
	//printf("Pressed: %u, Event: %s\n", gpio, event_mask==GPIO_IRQ_EDGE_RISE ? "rise" : "fall");
	uint64_t time = time_us_64();
	if (gpio==BUTTON_CONTROL_BRIGHTNESS) {
		if (time-button_brightness_last_accepted < BUTTON_DEBOUNCE_TIME) {
			return;
		}

		button_brightness_last_accepted = time;

		if (event_mask == GPIO_IRQ_EDGE_RISE) {
			if (lcd_backlight_on) {
				lcd_backlight_on = false;
			}
			else {
				lcd_backlight_start = time;
				lcd_backlight_on = true;
			}

			lcd_backlight_permanent = false;
			lcd.backlight(lcd_backlight_on);
		}
		else if (event_mask == GPIO_IRQ_EDGE_FALL) {
			if (lcd_backlight_on && time-lcd_backlight_start >= BUTTON_BACKLIGHT_PERMAON_TIME) {
				lcd_backlight_permanent = true;
			}
		}

		return;
	}

	if (event_mask == GPIO_IRQ_EDGE_FALL) {
		return;
	}
}

void setup_buttons() {
	gpio_init_mask(BUTTON_1|BUTTON_2|BUTTON_3);
	gpio_set_dir_in_masked(BUTTON_1|BUTTON_2|BUTTON_3);
	gpio_pull_down(BUTTON_1);
	gpio_pull_down(BUTTON_2);
	gpio_pull_down(BUTTON_3);

	gpio_set_irq_enabled_with_callback(BUTTON_1, GPIO_IRQ_EDGE_FALL|GPIO_IRQ_EDGE_RISE, true, button_callback);
	gpio_set_irq_enabled(BUTTON_2, GPIO_IRQ_EDGE_FALL|GPIO_IRQ_EDGE_RISE, true);
	gpio_set_irq_enabled(BUTTON_3, GPIO_IRQ_EDGE_FALL|GPIO_IRQ_EDGE_RISE, true);
}

uint64_t display_last_reset = 0;
uint64_t air_quality_last = 0;

void core1_main() {
	while (true) {
		// Reset display after every 10 min to control glitches in display
		if (time_us_64() - display_last_reset >= DISPLAY_RESET) {
			lcd.setup();
			display_last_reset = time_us_64();
		}

		// Measure air quality every second
		if (time_us_64() - air_quality_last >= SGP30_MEASUREMENT_DELAY && sgp30.measure_air_quality()) {
			air_quality_last = time_us_64();
		}

		// Update SGP30 humidity value every 5 seconds
		if (time_us_64() - lasthumidity > HUMIDITY_UPDATE) {
			lasthumidity = time_us_64();
			sgp30.set_relative_humidity(hdc1080.calculate_current_temperature(), hdc1080.calculate_current_humidity());

		}

		// Turn off backlight, if it's on, it's not permanent and time is up
		if (!lcd_backlight_permanent && lcd_backlight_on && time_us_64()-lcd_backlight_start > LCD_BRIGHT_TIME) {
			lcd_backlight_on = false;
			lcd.backlight(false);
		}

		sleep_ms(100);
	}
}

int main() {
	stdio_init_all();
	sleep_ms(2000);
	printf("Starting receiver\n");

	// Setup stuff
	setup_buttons();

	i2c = RP2040_I2C(I2C_INSTANCE, I2C_SDA_PIN, I2C_SCL_PIN, I2C_BAUDRATE);
	eeprom = AT93C46(AT93C46_CS, AT93C46_SK, AT93C46_DI, AT93C46_DO, spi1);
	hdc1080 = HDC1080(&i2c);
	sgp30 = SGP30(&eeprom, &i2c, !gpio_get(BUTTON_BASELINE_RESTART));
	lcd = ReceiverLCD(spi0, LCD_BACKLIGHT_PIN, LCD_CS_PIN, LCD_SCK_PIN, LCD_TX_PIN, LCD_CD_PIN, LCD_RST_PIN);

	rf_receiver_init(&ext_aht20_t, &ext_aht20_h, &ext_bmp280_t, &ext_bmp280_p);

	multicore_launch_core1(core1_main);
	
	while (true) {
		if (!hdc1080.measure()) {
			printf("Failed measuring HDC1080\n");
		}

		rf_read_message();
		lcd.update_outside_values((float)ext_bmp280_t/100, AHT20::calculate_humidity(ext_aht20_h), (float)ext_bmp280_p/25600);
		lcd.update_inside_air(sgp30.co2eq, sgp30.tvoc);
		lcd.update_inside_t_h(hdc1080.calculate_current_temperature(), hdc1080.calculate_current_humidity());
	}

	return 0;
}
