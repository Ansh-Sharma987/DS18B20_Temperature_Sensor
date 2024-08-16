#include <WiFi.h> // Library to enable WiFi connectivity on ESP32
#include <PubSubClient.h> // Library for MQTT communication
#include <OneWire.h> // Library to handle One-Wire communication
#include <DallasTemperature.h> // Library to interface with DS18B20 sensors

// Wifi Credentials
const char* ssid = "****"; // your ssid
const char* password = "****"; // your password

// MQTT broker address and credentials
const char* mqtt_server = "demo.thingsboard.io"; // 
int mqtt_port = 1883; // default port for MQTT  
const char* access_token = "**********"; // Access Token for your ThingsBoard Device

// GPIO where the DS18B20 is connected
#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS); // Create a OneWire object to communicate with the DS18B20 sensor
DallasTemperature sensors(&oneWire); // Pass the oneWire reference to DallasTemperature library

WiFiClient Client; // WiFiClient object to handle the connection to the network
PubSubClient client(Client); // PubSubClient object for MQTT communication

void WifiConnect() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Connect to Wifi

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void MQTTconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32", access_token, NULL)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WifiConnect();
  client.setServer(mqtt_server, mqtt_port);
  sensors.begin();
}

void loop() {
  if (!client.connected()) {
    MQTTconnect(); // Establish connection to ThingsBoard for MQTT communication
  }
  client.loop();

  sensors.requestTemperatures(); // Request Temperature reading from the sensor
  float temperature = sensors.getTempCByIndex(0); // Retrieve Temperature reading in Celsius
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  //Create a JSON payload
  String message = "{";
  message += "\"temperature\":";
  message += temperature;
  message += "}";

  // Publish temperature to ThingsBoard
  client.publish("v1/devices/me/telemetry", message.c_str());

  delay(30000); // Delay between readings
}