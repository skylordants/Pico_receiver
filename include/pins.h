#ifndef __PINS_H__
#define __PINS_H__

#include "hardware/i2c.h"

// EEPROM pins
#define AT93C46_CS 9
#define AT93C46_SK 10
#define AT93C46_DI 11
#define AT93C46_DO 12

// I2C configuration
#define I2C_SDA_PIN 26
#define I2C_SCL_PIN 27
#define I2C_BAUDRATE 400*1000 // 400 kHz
#define I2C_INSTANCE i2c1

// LCD pins
//#define LCD_RX_PIN 0
#define LCD_BACKLIGHT_PIN 2
#define LCD_TX_PIN 3
#define LCD_CS_PIN 5
#define LCD_SCK_PIN 6
#define LCD_CD_PIN 7
#define LCD_RST_PIN 8

#define SDA_PIN 26
#define SCL_PIN 27

// Button configuration
#define BUTTON_1 0
#define BUTTON_2 1
#define BUTTON_3 4

#define BUTTON_BASELINE_RESTART BUTTON_1
#define BUTTON_CONTROL_BRIGHTNESS BUTTON_1

#define BUTTON_DEBOUNCE_TIME 50000

#endif