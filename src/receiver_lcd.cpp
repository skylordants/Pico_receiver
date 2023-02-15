#include <stdio.h>

#include "receiver_lcd.h"

#define CO2EQ_THRESHOLD 1000
#define TVOC_THRESHOLD 350

ReceiverLCD::ReceiverLCD(spi_inst_t *spi, uint8_t pBL, uint8_t pCS, uint8_t pSCK, uint8_t pTX, uint8_t pCD, uint8_t pRST)
	: ST7565R(spi, pBL, pCS, pSCK, pTX, pCD, pRST)
{
	setup();
}

ReceiverLCD::ReceiverLCD() {
	
}

bool ReceiverLCD::setup() {
	reset();

	char buf[40] = "hPaC%OUTSIDE:INSIDE:ppm co2ppb tvoc";
	write_chars(buf, 3, 5, 20);
	write_chars(buf+3, 1, 6, 9);
	write_chars(buf+4, 1, 6, 20);
	write_chars(buf+5, 8, 7, 0);

	ram_select_address(6, 8*5);
	ram_write(deg, 5);

	write_chars(buf+3, 1, 2, 9);
	write_chars(buf+4, 1, 2, 20);
	write_chars(buf+13, 7, 3, 0);

	ram_select_address(2, 8*5);
	ram_write(deg, 5);

	write_chars(buf+20, 7, 1, 14);
	write_chars(buf+27, 8, 0, 14);

	return true;
}

bool ReceiverLCD::update_outside_values(float temperature, float humidity, float pressure) {
	char t[10], h[10], p[10];

	snprintf(t, sizeof t, "%f", temperature);
	snprintf(h, sizeof h, "%f", humidity);
	snprintf(p, sizeof p, "%f", pressure);

	write_chars(t, 5, 6, 3);
	write_chars(h, 5, 6, 14);
	write_chars(p, 5, 5, 14);
	return true;
}

bool ReceiverLCD::update_inside_t_h(float temperature, float humidity) {
	char t[10], h[10];

	snprintf(t, sizeof t, "%f", temperature);
	snprintf(h, sizeof h, "%f", humidity);

	write_chars(t, 5, 2, 3);
	write_chars(h, 5, 2, 14);
	return true;
}


bool ReceiverLCD::update_inside_air(uint16_t co2eq, uint16_t tvoc) {
	char c[11], v[11];

	snprintf(c, sizeof c, "%10u", co2eq);
	snprintf(v, sizeof v, "%10u", tvoc);

	write_chars(c, 10, 1, 3);
	write_chars(v, 10, 0, 3);

	char s[3] = "! ";

	if (co2eq > CO2EQ_THRESHOLD) {
		write_chars(s, 1, 1, 23);
	}
	else {
		write_chars(s+1, 1, 1, 23);
	}

	if (tvoc > TVOC_THRESHOLD) {
		write_chars(s, 1, 0, 23);
	}
	else {
		write_chars(s+1, 1, 0, 23);
	}
	return true;
}