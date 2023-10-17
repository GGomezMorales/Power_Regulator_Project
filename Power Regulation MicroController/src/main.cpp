#include <Arduino.h>

// Pinout ESP32 Wemos Lolin32: GPIO 16, 34, 35
const byte gateTriacPin = 16;
const byte zeroCrossingPin = 34;

// Variables
unsigned long delayTime = 0; // Delay time in microseconds between the zero crossing and the gate signal.
volatile bool zeroCrossed = false;

void handleZeroCrossing()
{
  zeroCrossed = true;
}

void setup()
{
  Serial.begin(115200);
  pinMode(gateTriacPin, OUTPUT);
  pinMode(zeroCrossingPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(zeroCrossingPin), handleZeroCrossing, FALLING);

}
  
void loop()
{
  // angle = 90;

  if (zeroCrossed)
  {
    digitalWrite(gateTriacPin, HIGH);
    delayMicroseconds(50);
    digitalWrite(gateTriacPin, LOW);
    zeroCrossed = false;
  }
}
