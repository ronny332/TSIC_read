# TSIC_read
inspired by [arduino-tsic](https://github.com/Schm1tz1/arduino-tsic) this repository was created.
It is definitely not finished ;-) ... feel free to fetch some impressions from it, but currently it is nothing more than a testing environment for me.

**Use this source code to start an Arduino Sketch**

`#include <TSIC_read.h>

float temperature;
uint16_t value;

char* buf = (char*) malloc(12);
TSIC_read *readers[2];

void setup() {
  Serial.begin(9600);

   // 13 -> data pin, 12 -> VDD pin
   readers[0] = new TSIC_read(13,12);
   readers[1] = new TSIC_read(11,10);
}

void loop() {
  for (uint8_t i = 0; i < sizeof(readers) / sizeof(TSIC_read*); i++) {
    value = readers[i]->read_sensor();
    temperature = readers[i]->calc_celsius(&value);

    sprintf(buf, "Sensor #%d: ", i);
    Serial.print(buf);

    if (!isnan(temperature))
      Serial.println(temperature);
    else
      Serial.println(F("error"));
  }

  delay(1000);
}`