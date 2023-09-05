#include <WiFi.h>
#include <PubSubClient.h>

const char *ssid = "name";
const char *password = "noquiero";
const char *mqttServer = "broker.emqx.io";
const int mqttPort = 1883;
const char *mqttUser = "namex";
const char *mqttPassword = "noquierox";

const int ledPin = 16;

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a la red WiFi");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected())
  {
    Serial.println("Conectando al broker MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword))
    {
      Serial.println("Conectado al broker MQTT");
      client.subscribe("led/control");
    }
    else
    {
      Serial.print("Fallo al conectar al broker MQTT. Estado: ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Mensaje recibido en el topic: ");
  Serial.println(topic);

  if ((char)payload[0] == '1')
  {
    digitalWrite(ledPin, HIGH); // Enciende el LED
  }
  else if ((char)payload[0] == '0')
  {
    digitalWrite(ledPin, LOW); // Apaga el LED
  }
}

void loop()
{
  client.loop();
}
