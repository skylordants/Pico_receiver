#ifndef __RECEIVER_LCD_H__
#define __RECEIVER_LCD_H__

#include "st7565r.h"

class ReceiverLCD : public ST7565R {
public:
	ReceiverLCD(spi_inst_t *spi, uint8_t pBL, uint8_t pCS, uint8_t pSCK, uint8_t pTX, uint8_t pCD, uint8_t pRST);
	ReceiverLCD();

	bool setup();
	bool update_outside_values(float temperature, float humidity, float pressure);
	bool update_inside_t_h(float temperature, float humidity);
	bool update_inside_air(uint16_t co2eq, uint16_t tvoc);
};

#endif