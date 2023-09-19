#include <Arduino.h>

// Pinout ESP32 Wemos Lolin32: GPIO 16, 34, 35
const byte gateTriacPin = 16;
const byte zeroCrossingPin = 34;
const byte anglePin = 35; // This pin will later be replaced by an angle sent from the app.

// Variables
int delayTime = 0;      // Delay time in microseconds between the zero crossing and the gate signal.
volatile int angle = 0; // Angle of the potentiometer (0° - 180°)
volatile bool zeroCrossed = false;
volatile int counter = 0;

void zeroCrossing()
{
  zeroCrossed = true;
}

void setup()
{
  Serial.begin(115200);
  pinMode(gateTriacPin, OUTPUT);
  pinMode(zeroCrossingPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(zeroCrossingPin), zeroCrossing, FALLING);
}

void loop()
{
  int angleValue = analogRead(anglePin);
  angle = map(angleValue, 0, 4095, 0, 180);

  if (zeroCrossed)
  {
    int delayTime = map(angle, 0, 180, 0, 8333);
    delayMicroseconds(delayTime);
    digitalWrite(gateTriacPin, HIGH);
    delayMicroseconds(50);
    digitalWrite(gateTriacPin, LOW);
    counter++;
    zeroCrossed = false;
  }

  Serial.print("Counter: ");
  Serial.println(counter);
  Serial.print("Angle: ");
  Serial.println(angle);
  delay(50);
}
