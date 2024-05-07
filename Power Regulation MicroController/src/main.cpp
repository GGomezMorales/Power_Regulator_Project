#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>

const char *ssid = "HOME RD-SOMOS_2.4g";
const char *password = "F10773929";

const int webSocketPort = 81;
WebSocketsServer webSocket = WebSocketsServer(webSocketPort);

const byte gateTriacPin = 16;
const byte zeroCrossingPin = 34;
const byte currentSensorPin = 33;  // Pin del sensor de corriente

int angle = 90;
unsigned long delayTime = 0;
volatile bool zeroCrossed = false;
const float offsetVoltage = 2.40; // Asumiendo que midiste esto sin carga
const float sensitivity = 0.126;  // Ajusta este valor basado en tus mediciones con 3.3V

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
      angle = constrain(angle, 12, 156);
      break;
  }
}

float readCurrent() {
    static float lastReadings[10];
    static int index = 0;
    int analogValue = analogRead(currentSensorPin);
    float voltage = (analogValue * 3.3) / 4095.0; // Asumiendo que usas la referencia de 3.3V

    // Filtrar el ruido con un promedio móvil
    lastReadings[index] = voltage;
    index = (index + 1) % 10; // Incrementar el índice de manera circular

    float averageVoltage = 0;
    for (int i = 0; i < 10; i++) {
        averageVoltage += lastReadings[i];
    }
    averageVoltage /= 10;

    float current = (averageVoltage - offsetVoltage) / sensitivity; // Calcular corriente basada en voltaje y sensibilidad
    return current;
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a la red WiFi...");
  }

  Serial.print("Dirección IP de la ESP32: ");
  Serial.println(WiFi.localIP());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  pinMode(gateTriacPin, OUTPUT);
  pinMode(zeroCrossingPin, INPUT_PULLDOWN);
  pinMode(currentSensorPin, INPUT); // Configurar pin del sensor de corriente como entrada
  attachInterrupt(digitalPinToInterrupt(zeroCrossingPin), handleZeroCrossing, RISING);

  Serial.println("Servidor WebSocket iniciado");
}

float readVoltage() {
    int analogValue = analogRead(currentSensorPin);
    float voltage = (analogValue * 3.3) / 4095.0;
    return voltage;
}

void loop() {
  // Control del Triac basado en cruce por cero
  if (zeroCrossed) {
    zeroCrossed = false;  // Reiniciar la bandera de cruce por cero

    // Calcular el tiempo de retardo basado en el ángulo de disparo
    delayMicroseconds(50);
    delayTime = map(angle, 0, 180, 8333, 0);
    delayMicroseconds(delayTime);

    // Activar el triac
    digitalWrite(gateTriacPin, HIGH);
    delayMicroseconds(100);  // Duración del pulso de encendido
    digitalWrite(gateTriacPin, LOW);
  }

  // Leer la corriente constantemente
  float voltage = readVoltage();
  float current = (voltage - offsetVoltage) / sensitivity;

  // Imprimir los valores de voltaje y corriente
  Serial.print("Voltaje: ");
  Serial.print(voltage);
  Serial.print(", Corriente: ");
  Serial.println(current, 3);

  delay(2000);  // Retardo para facilitar la visualización

  webSocket.loop();
}


