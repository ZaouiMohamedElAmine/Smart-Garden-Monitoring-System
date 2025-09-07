#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <DHT.h>

// Project: Smart Garden Monitoring System
// Description: IoT system that monitors temperature, humidity, and soil moisture,
// then transmits the data to a cloud dashboard for remote monitoring.

// WiFi credentials
char ssid[] = "your_network";
char pass[] = "your_password";

// MQTT broker details
char mqttBroker[] = "broker.hivemq.com";
int mqttPort = 1883;
char topic[] = "smartGarden/data";

// Sensor pins
#define DHTPIN 2
#define DHTTYPE DHT22
#define MOISTURE_PIN A0

// Initialize components
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
DHT dht(DHTPIN, DHTTYPE);

// Variables for sensor data
float temperature = 0;
float humidity = 0;
int soilMoisture = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  // Connect to WiFi
  connectToWiFi();
  
  // Connect to MQTT broker
  connectToMQTT();
}

void loop() {
  // Read sensor data every 10 seconds
  readSensorData();
  
  // Publish data to MQTT
  publishData();
  
  // Maintain MQTT connection
  mqttClient.poll();
  
  delay(10000);
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);
  }
  Serial.println("\nConnected to WiFi!");
}

void connectToMQTT() {
  Serial.print("Connecting to MQTT broker...");
  if (!mqttClient.connect(mqttBroker, mqttPort)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }
  Serial.println("\nConnected to MQTT broker!");
}

void readSensorData() {
  // Read temperature and humidity
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  
  // Read soil moisture (0-1023, lower means drier)
  soilMoisture = analogRead(MOISTURE_PIN);
}

void publishData() {
  // Create JSON payload
  String payload = "{";
  payload += "\"temperature\":" + String(temperature) + ",";
  payload += "\"humidity\":" + String(humidity) + ",";
  payload += "\"soilMoisture\":" + String(soilMoisture);
  payload += "}";
  
  // Publish to MQTT topic
  mqttClient.beginMessage(topic);
  mqttClient.print(payload);
  mqttClient.endMessage();
  
  Serial.println("Data published: " + payload);
}