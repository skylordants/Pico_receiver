// Inspired by https://github.com/wagiminator/ATtiny13-TinyRemoteRF/blob/main/software/remote_4_buttons/RF_Remote_4B.ino

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/timer.h"
#include "hardware/adc.h"

#define RF_BIT 150  //The symbol bit length in us
#define RF_SYMBOL 4*RF_BIT  //The symbol length
#define RF_ZERO 1*RF_BIT
#define RF_ONE 3*RF_BIT
#define RF_REP 3
#define RF_REP_PAUSE 2*RF_SYMBOL

#define RF_PREAMPLE 16  //Number of 1 bits before a transmission
#define RF_START 4  //Number of 0 and 1 bit pairs before a packet

#define RECEIVER_PIN 14


bool read_bit(uint8_t *bit) {
  bool continuous = true;

  uint32_t begin = time_us_32();
  while (gpio_get(RECEIVER_PIN) == 0) {
    sleep_us(RF_BIT/2);
  }
  uint32_t delay = time_us_32() - begin;
  

  if (delay > 2*RF_SYMBOL) {
    continuous = false;
  }
  
  sleep_us(2*RF_BIT);
  if (gpio_get(RECEIVER_PIN)) {
    *bit = 1;
  }
  else {
    *bit = 0;
  }

  sleep_us(RF_BIT);
  return continuous;
}

bool read_byte(uint8_t *byte) {
  *byte = 0;

  for (int i = 7; i >= 0; i--) {
    uint8_t bit = 0;
    if (read_bit(&bit) == false) {
      return false;
    }
    *byte |= bit <<i;
  }

  return true;
}

bool read_message() {
  // Preample
  int captured_ones = 0;
  uint8_t last_bit = 0;

  while (captured_ones < 8 || last_bit == 1) {
    if (read_bit(&last_bit) && (last_bit == 1 || captured_ones >= 8)) {
      captured_ones++;
    }
    else {
      captured_ones = 0;
    }
  }

  // Start
  int captured_start = 1;

  while (captured_start < 2*RF_START) {
    uint8_t new_bit = 0;
    if (read_bit(&new_bit) && new_bit != last_bit) {
      last_bit = new_bit;
      captured_start++;
    }
    else {
      return false;
    }
  }

  // Packet length
  uint8_t len = 0;
  if (read_byte(&len) == false) {
    return false;
  }

  
  // Message
  std::vector <uint8_t> buffer;
  for (int i = 0; i < len; i++) {
    uint8_t byte = 0;
    if (read_byte(&byte) == false) {
      return false;
    }
    buffer.push_back(byte);
  }

  for (int i = 0; i < len; i++) {
    printf("%c", buffer[i]);
  }
  return true;
}


int main() {
  // Setup stuff
  stdio_init_all();

  gpio_init(RECEIVER_PIN);
  gpio_set_dir(RECEIVER_PIN, GPIO_IN);
  gpio_pull_down(RECEIVER_PIN);

  while (true) {
    read_message();
  }

  return 0;
}
