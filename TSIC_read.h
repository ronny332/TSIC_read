//
// Created by noname on 22.10.15.
//

#ifndef AVR_TSIC206_TSIC_READ_H
#define AVR_TSIC206_TSIC_READ_H

#define FAILED(pin_vdd)                                                        \
  digitalWriteFast(pin_vdd, 0);                                                \
  return NAN;

#define MAX_TIMEOUTS 2500

#define TIMEOUT()                                                              \
  timeout--;                                                                   \
  if (timeout == 0)                                                            \
    return NAN;

#include <Arduino.h>
#include "digitalWriteFast.h"

float calc_celsius(uint16_t *temperature16);
uint8_t check_parity(int16_t *temp_value);
uint8_t read_byte(uint8_t pin_data, uint8_t frame);
uint16_t read_sensor(uint8_t pin_data, uint8_t pin_vdd);

class TSIC_read {
public:
  explicit TSIC_read(uint8_t pin_data, uint8_t pin_vdd);
  
  float calc_celsius(uint16_t *temperature16);
  uint16_t read_sensor();

private:
  uint8_t check_parity(int16_t *temp_value);
  uint8_t read_byte(uint8_t frame);
  
  int16_t  *p_sensor_data;
  int8_t   pin_data;
  int8_t   pin_vdd;
  int16_t  sensor_data;
  int16_t  sensor_data_0;
  int16_t  sensor_data_1;
  uint8_t  strobe_counter;
  uint8_t  strobe_length;
  uint16_t timeout;
};

#endif // AVR_TSIC206_TSIC_READ_H
