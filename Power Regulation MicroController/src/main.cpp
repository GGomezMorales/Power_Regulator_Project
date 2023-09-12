#include <Arduino.h>

const byte gateTriacPin = 16;
const byte zeroCrossingPin = 34;
const byte anglePin = 35;
const int delayTime = 0;
volatile int angle = 0;
volatile bool zeroCrossed = false;
int counter = 0;
unsigned long lastZeroCrossingTime = 0;
const unsigned long debounceDelay = 100; // Adjust this value as needed

void zeroCrossing()
{
  unsigned long currentMillis = millis();

  // Check if enough time has passed since the last zero-crossing event
  if (currentMillis - lastZeroCrossingTime >= debounceDelay)
  {
    Serial.println(currentMillis - lastZeroCrossingTime);
    zeroCrossed = true;
    lastZeroCrossingTime = currentMillis;
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(gateTriacPin, OUTPUT);
  pinMode(zeroCrossingPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(zeroCrossingPin), zeroCrossing, RISING);
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
    counter++;

    Serial.println("Datos dentro del if");

    Serial.print("Counter: ");
    Serial.println(counter);

    Serial.print("Angle: ");
    Serial.println(angle);

    Serial.print("Potenciometer: ");
    Serial.println(angleValue);

    Serial.print("Zero Crossing: ");
    Serial.println(zeroCrossed);

    Serial.print("Delay Time: ");
    Serial.println(delayTime);

    Serial.println(" ");

    delayMicroseconds(2000000);
    digitalWrite(gateTriacPin, LOW);
    zeroCrossed = false;
  }

  Serial.println("Datos fuera del if");

  Serial.print("Counter: ");
  Serial.println(counter);

  Serial.print("Angle: ");
  Serial.println(angle);

  Serial.print("Potenciometer: ");
  Serial.println(angleValue);

  Serial.print("Zero Crossing: ");
  Serial.println(zeroCrossed);

  Serial.print("Delay Time: ");
  Serial.println(delayTime);

  Serial.println(" ");

  delay(500);
}
