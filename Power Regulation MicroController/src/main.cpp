#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>

const char *ssid = "rwork";
const char *password = "MoniP3P3";

const int webSocketPort = 81;
WebSocketsServer webSocket = WebSocketsServer(webSocketPort);

const byte gateTriacPin = 16;
const byte zeroCrossingPin = 34;

int angle = 90;
unsigned long delayTime = 0;
volatile bool zeroCrossed = false;

void handleZeroCrossing()
{
  zeroCrossed = true;
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Desconectado!\n", num);
      break;
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Conectado desde %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
    }
    break;
    case WStype_TEXT:
      Serial.printf("[%u] Recibido: %s\n", num, payload);
      angle = atoi((char *)payload); 
      angle = constrain(angle, 0, 180);
      break;
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a la red WiFi...");
  }

  // Muestra la dirección IP de la ESP32 en el monitor serie
  Serial.print("Dirección IP de la ESP32: ");
  Serial.println(WiFi.localIP());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  pinMode(gateTriacPin, OUTPUT);
  pinMode(zeroCrossingPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(zeroCrossingPin), handleZeroCrossing, RISING);

  Serial.println("Servidor WebSocket iniciado");
}

void loop() {
  if (zeroCrossed) {
    delayMicroseconds(50);
    delayTime = map(angle, 0, 180, 8333, 0);
    delayMicroseconds(delayTime);
    digitalWrite(gateTriacPin, HIGH);
    delayMicroseconds(100); // Duración del pulso de encendido (ajústalo según tus necesidades).
    digitalWrite(gateTriacPin, LOW);
    zeroCrossed = false;
  }

  webSocket.loop();
}
