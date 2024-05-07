#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>

const char *ssid = "HOME RD-SOMOS_2.4g";
const char *password = "F10773929";

const int webSocketPort = 81;
WebSocketsServer webSocket = WebSocketsServer(webSocketPort);

const byte gateTriacPin = 16;
const byte zeroCrossingPin = 34;
const int sensorPin = A0;  // Assuming the sensor is connected to pin A0
const int numSamples = 10;
float readings[numSamples];
int readIndex = 0;

int angle = 90;
unsigned long delayTime = 0;
volatile bool zeroCrossed = false;

void handleZeroCrossing() {
  zeroCrossed = true;
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  Serial.printf("Event: %d\n", type);
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      break;
    }
    case WStype_TEXT:
      Serial.printf("[%u] Received: %s\n", num, payload);
      angle = atoi((char *)payload);
      angle = constrain(angle, 12, 156);
      break;
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  pinMode(gateTriacPin, OUTPUT);
  pinMode(zeroCrossingPin, INPUT_PULLDOWN);
  pinMode(sensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(zeroCrossingPin), handleZeroCrossing, RISING);

  Serial.println("WebSocket server started");
}

float readVoltage() {
  int analogValue = analogRead(sensorPin);
  return (analogValue * 3.3) / 4095.0; // Convert analog value to voltage
}

float filterVoltage(float newReading) {
  readings[readIndex] = newReading;
  readIndex = (readIndex + 1) % numSamples;

  float sum = 0.0;
  for (int i = 0; i < numSamples; i++) {
    sum += readings[i];
  }
  return sum / numSamples; // Return the average of readings
}

void loop() {
  webSocket.loop();
  if (zeroCrossed) {
    zeroCrossed = false;
    delayMicroseconds(50);
    delayTime = map(angle, 0, 180, 8333, 0);
    delayMicroseconds(delayTime);
    digitalWrite(gateTriacPin, HIGH);
    delayMicroseconds(100);
    digitalWrite(gateTriacPin, LOW);
  }

  float voltage = readVoltage();
  float filteredVoltage = filterVoltage(voltage);
  String voltageString = String(filteredVoltage, 3); // Convert filtered voltage to string with 3 decimal places
  webSocket.broadcastTXT(voltageString.c_str()); // Broadcast the voltage string to all connected clients
}
