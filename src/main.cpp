// Inspired by https://github.com/wagiminator/ATtiny13-TinyRemoteRF/blob/main/software/remote_4_buttons/RF_Remote_4B.ino

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/spi.h"

#include "lcd.h"
#include "rf_receiver.h"
#include "aht20.h"
#include "hdc1080.h"
#include "i2c.h"

int main() {
	stdio_init_all();
	sleep_ms(2000);
	printf("Starting receiver\n");
	// Setup stuff
	i2c_setup();
	lcd_setup(spi0);

	lcd_backlight(true);

	uint32_t ext_aht20_t = 0;
	uint32_t ext_aht20_h = 0;
	int32_t ext_bmp280_t = 0;
	uint32_t ext_bmp280_p = 0;

	uint16_t hdc1080_t = 0;
	uint16_t hdc1080_h = 0;

	rf_receiver_init(&ext_aht20_t, &ext_aht20_h, &ext_bmp280_t, &ext_bmp280_p);

	lcd_hud_setup();

	hdc1080_setup();

	while (true) {
		if (!hdc1080_measure(&hdc1080_t, &hdc1080_h)) {
			printf("Failed measuring HDC1080\n");
		}
		rf_read_message();
		lcd_hud_update_outside_values((float)ext_bmp280_t/100, aht20_calculate_humidity(ext_aht20_h), (float)ext_bmp280_p/25600);

		lcd_hud_update_inside_values(hdc1080_calculate_temperature(hdc1080_t), hdc1080_calculate_humidity(hdc1080_h));
	}

	return 0;
}
