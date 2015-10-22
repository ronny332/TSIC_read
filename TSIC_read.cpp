//
// Created by noname on 22.10.15.
//

#include "TSIC_read.h"

TSIC_read::TSIC_read(uint8_t pin_data, uint8_t pin_vdd)
    : pin_data(pin_data), pin_vdd(pin_vdd) {
  pinMode(pin_vdd, OUTPUT);
  pinMode(pin_data, INPUT);
}

uint8_t TSIC_read::check_parity(int16_t *raw_data) {
  uint8_t parity = 0;

  for (uint8_t i = 0; i < 9; i++) {
    if (*raw_data & (1 << i))
      parity++;
  }

  if (parity % 2)
    // wrong parity
    return 0;

  // delete parity bit
  *raw_data >>= 1;
  return 1;
}

float TSIC_read::calc_celsius(uint16_t *temperature16) {
  if (*temperature16 <= 0)
    return NAN;

  uint16_t temp_value16 = 0;
  float celsius = 0;

  // calculate temperature *10, i.e. 26,4 = 264
  temp_value16 = ((*temperature16 * 250L) >> 8) - 500;

  // shift comma by 1 digit e.g. 26,4°C
  celsius = temp_value16 / 10 + (float)(temp_value16 % 10) / 10;
  return celsius / 2;
}

uint8_t TSIC_read::read_byte(uint8_t frame) {
  p_sensor_data = (frame == 0) ? &sensor_data_0 : &sensor_data_1;

  strobe_counter = 0;
  strobe_length = 0;
  timeout = MAX_TIMEOUTS;

  // wait for falling edge
  while (digitalReadFast(pin_data)) {
    delayMicroseconds(10);
    TIMEOUT();
  }

  // measure strope time
  while (!digitalReadFast(pin_data)) {
    strobe_length++;
    delayMicroseconds(10);
    TIMEOUT();
  }

  // read bits
  for (uint8_t i = 0; i < 9; i++) {
    // wait for falling edge
    while (digitalReadFast(pin_data)) {
      delayMicroseconds(10);
      TIMEOUT();
    }

    // wait strobe time
    strobe_counter = strobe_length;

    while (strobe_counter > 0) {
      delayMicroseconds(10);
      strobe_counter--;
      TIMEOUT();
    }

    *p_sensor_data <<= 1;
    if (digitalReadFast(pin_data)) {
      *p_sensor_data |= 1;
    }

    // wait for rising edge
    while (!digitalReadFast(pin_data)) {
      delayMicroseconds(10);
      TIMEOUT();
    }
  }

  return 1;
}

uint16_t TSIC_read::read_sensor() {
  sensor_data = 0;
  sensor_data_0 = 0;
  sensor_data_1 = 0;

  // sensor ON
  digitalWriteFast(pin_vdd, HIGH);
  // let the sensor initialize
  delayMicroseconds(50);

  // frame 0
  if (!read_byte(0)) {
    FAILED(pin_vdd);
  }
  // frame 1
  if (!read_byte(1)) {
    FAILED(pin_vdd);
  }

  // sensor OFF
  digitalWriteFast(pin_vdd, LOW);

  // parity check
  if (!check_parity(&sensor_data_0))
    return 0;
  if (!check_parity(&sensor_data_1))
    return 0;

  sensor_data = (sensor_data_0 << 8) | sensor_data_1;

  return sensor_data;
}