// Inspired by https://github.com/wagiminator/ATtiny13-TinyRemoteRF/blob/main/software/remote_4_buttons/RF_Remote_4B.ino

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/spi.h"

#include "lcd.h"
#include "rf_receiver.h"
#include "aht20.h"


// Template values: 19.360001°C, 99657.656250 Pa, 15.325737%
float t = 19.360001f;
float h = 15.325737f;
float p = 996.57656250f;

int main() {
	stdio_init_all();
	sleep_ms(2000);
	printf("Starting receiver\n");
	// Setup stuff
	lcd_setup(spi0);
	lcd_backlight(true);

	uint32_t aht20_t = 0;
	uint32_t aht20_h = 0;
	int32_t bmp280_t = 0;
	uint32_t bmp280_p = 0;

	rf_receiver_init(&aht20_t, &aht20_h, &bmp280_t, &bmp280_p);

	lcd_hud_setup();
	lcd_hud_update_inside_values(t, h, p);

	while (true) {
		rf_read_message();
		lcd_hud_update_outside_values((float)bmp280_t/100, aht20_calculate_humidity(aht20_h), (float)bmp280_p/25600);
	}

	return 0;
}
