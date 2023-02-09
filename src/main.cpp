// Inspired by https://github.com/wagiminator/ATtiny13-TinyRemoteRF/blob/main/software/remote_4_buttons/RF_Remote_4B.ino

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/spi.h"
#include "pico/time.h"

#include "pins.h"

#include "lcd.h"
#include "rf_receiver.h"
#include "aht20.h"
#include "hdc1080.h"
#include "i2c.h"
#include "sgp30.h"
#include "humidity.h"
#include "at93c46.h"

#define HUMIDITY_UPDATE 5000000

uint32_t ext_aht20_t = 0;
uint32_t ext_aht20_h = 0;
int32_t ext_bmp280_t = 0;
uint32_t ext_bmp280_p = 0;

uint16_t hdc1080_t = 0;
uint16_t hdc1080_h = 0;

uint16_t sgp30_co2 = 0;
uint16_t sgp30_tvoc = 0;

SGP30 sgp30;
HDC1080 hdc1080;

AT93C46 eeprom;

uint64_t lasthumidity = 0;
uint64_t lastbaseline = 0;

void core1_main() {
	while (true) {
		if (sgp30.measure_air_quality(&sgp30_co2, &sgp30_tvoc)) {
			lcd_hud_update_inside_air(sgp30_co2, sgp30_tvoc);
		}

		if (time_us_64() - lasthumidity > HUMIDITY_UPDATE) {
			lasthumidity = time_us_64();
			sgp30.set_humidity(AH_for_sgp30(HDC1080::calculate_temperature(hdc1080_t), HDC1080::calculate_humidity(hdc1080_h)));

		}


		sleep_ms(1000);
	}
}

int main() {
	stdio_init_all();
	sleep_ms(2000);
	printf("Starting receiver\n");
	// Setup stuff
	i2c_setup();

	eeprom = AT93C46(AT93C46_CS, AT93C46_SK, AT93C46_DI, AT93C46_DO, spi1);
	hdc1080 = HDC1080(0);
	sgp30 = SGP30(&eeprom, !gpio_get(BUTTON_BASELINE_RESTART));

	lcd_setup(spi0);
	lcd_backlight(true);
	lcd_hud_setup();

	rf_receiver_init(&ext_aht20_t, &ext_aht20_h, &ext_bmp280_t, &ext_bmp280_p);

	multicore_launch_core1(core1_main);
	

	while (true) {
		if (!hdc1080.measure(&hdc1080_t, &hdc1080_h)) {
			printf("Failed measuring HDC1080\n");
		}

		rf_read_message();
		lcd_hud_update_outside_values((float)ext_bmp280_t/100, aht20_calculate_humidity(ext_aht20_h), (float)ext_bmp280_p/25600);

		lcd_hud_update_inside_t_h(HDC1080::calculate_temperature(hdc1080_t), HDC1080::calculate_humidity(hdc1080_h));
	}

	return 0;
}
