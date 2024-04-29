#include <Arduino.h>
#include "interrupt.h"


void setup()
{
  Serial.begin(115200);
  _uart.init_driver_uart();
  
}

void loop()
{
  Serial.println("loop");
  _uart.functionOne();
  delay(5000);

}