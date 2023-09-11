#include <Arduino.h>

const int anglePin = 34;
const int ledPin = 16;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  int angleValue = analogRead(anglePin);
  int dutyCycle = map(angleValue, 0, 4095, 0, 255);

  analogWrite(ledPin, dutyCycle);
  delay(50);

  // Imprime los valores en el Serial Monitor
  Serial.print("Valor del potenciómetro: ");
  Serial.print(angleValue);
  Serial.print("\t Ciclo de trabajo del LED: ");
  Serial.println(dutyCycle);
}
