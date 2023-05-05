#include "secrets.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <ThingSpeak.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <esp_sleep.h>
#include "WiFi.h"
#include "DHT.h"

// AWS IoT
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

// ThingSpeak
unsigned long channelID = 2130323; // Change this to your channel ID
const char* writeAPIKey = "8RGA25BI3RXD8KKK"; // Change this to your write API key

// pin
#define DHT_PIN 27
#define PHOTCELL_PIN 36
const gpio_num_t WAKEUP_PIN = GPIO_NUM_35;
const int relayPin = 33;
const int moisturePin = 37;


// moisture sensor
const int dryValue = 150;
const int wetValue = 900;
int moistureLevel;
int percentMoisture;


// DHT sensor
float h ;
float t;

// photocell
int photocellValue;
int brightness;

// Weather forecast
unsigned long millis_until_midnight;
String weather_status;


WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
WiFiClient client2;

// Create a DHT object
DHT dht(DHT_PIN, DHT21);


int readMoisture(int moisturePin) {
  // Read the moisture level from the sensor
  int total = 0;
  for (int i = 0; i < 100; i++) {
    moistureLevel = analogRead(moisturePin);
    total += moistureLevel;
    delay(100);
  }
  // Average the 100 readings for a more stable moisture value
  moistureLevel = total / 100;
  
  // Convert the analog reading to a percentage
  percentMoisture = map(moistureLevel, dryValue, wetValue, 0, 100);
  
  // Return the moisture level as an integer
  return (int)percentMoisture;
}

int photocell_calibrate() {
  int total = 0;
  int mapped;
  for (int i = 0; i < 100; i++) {
    photocellValue = analogRead(PHOTCELL_PIN);
    total += photocellValue;
    delay(10);
  }
  photocellValue = total / 100;

  mapped = map(photocellValue, 400, 4095, 0, 100);

  return mapped;
}

String get_weather() {
  String status;
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/forecast?q=San%20Diego&appid=" "&units=metric"; // Change this to your city, replace " " (include quotes) to your APIkey
  http.begin(url);

  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    // Serial.println(payload);

    const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(10) + 260;
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, payload);

    JsonArray list = doc["list"];
    for (JsonObject obj : list) {
      float temp = obj["main"]["temp"];
      float humidity = obj["main"]["humidity"];
      String w_status = obj["weather"][0]["main"];

      // Serial.print("Temperature: ");
      // Serial.println(temp);
      // Serial.print("Humidity: ");
      // Serial.println(humidity);
      status = w_status;
      Serial.print("Status: ");
      Serial.println(status);
      Serial.println();
    }
  }

  http.end();
  return status;
}

void water() {
  // Read the moisture level from the sensor
  moistureLevel = readMoisture(moisturePin);
  Serial.println("Moisture level: " + String(percentMoisture) + "% ");
  

  // If the moisture level is below 30%, water the plant
  if (moistureLevel < 30) {
    Serial.println("Low moisture level!");
    // Water the plant until the moisture level is above 60%
    while (percentMoisture < 60) {
      Serial.print("Watering the plant... => ");
      percentMoisture = readMoisture(moisturePin);
      Serial.println("Moisture: " + String(percentMoisture) + "% ");
      digitalWrite(relayPin, HIGH); // turn on the relay
      delay(3000); // wait for 3 seconds
      digitalWrite(relayPin, LOW); // turn off the relay
      delay(1000); // wait for 1 seconds
    }
  } else {
    Serial.println("Good moisture level!");
  }
  delay(5000); // wait for 5 seconds
}

void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Incoming: ");
  Serial.println(topic);
 
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);

  // Water the plant if the message is "water"
  if (message = "water" ) {
    // Read the moisture level from the sensor
    int percentMoisture = readMoisture(moisturePin);
    if (message == "water" && percentMoisture < 60) {
      water();
    }
  }
}

void connect_WIFI() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println("Connecting to Wi-Fi");
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
}

void connect_AWS()
{
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
 
  // Create a message handler
  client.setCallback(messageHandler);
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }
 
  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }
 
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  Serial.println("AWS IoT Connected!");
}
 
void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["humidity"] = h;
  doc["temperature"] = t;
  doc["moisture"] = percentMoisture;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  Serial.println("Published message");
  Serial.println();
}

void setup()
{
  Serial.begin(115200);
  delay(3000);
  // Conect to Wifi
  connect_WIFI();
  // Connect to AWS
  connect_AWS();
  // Initialize ThingSpeak
  ThingSpeak.begin(client2);
  // Initialize the DHT sensor
  dht.begin();
  // Set up the relay pin
  pinMode(relayPin, OUTPUT);
  // Set the photocell pin as input
  pinMode(PHOTCELL_PIN, INPUT);
  // Set up the button pin
  pinMode(WAKEUP_PIN, INPUT_PULLUP);
}
 
void loop()
{
  // Get the weather status
  Serial.println("Getting weather status...");
  weather_status = get_weather();

  // // If it's raining, go to sleep for 2 hours
  if (weather_status == "Rain") {
    Serial.println("It's raining! Going to sleep for 2 hours...");
    // Configure the ESP32 to wake up from deep sleep mode on a timer
    esp_sleep_enable_timer_wakeup((int64_t)2 * 3600 * 1000000); // Wake up in 2 hours
    esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, LOW); // Wake up when the button is pressed
    delay(100); // Wait for the serial buffer to flush
    esp_deep_sleep_start();
  }

  // Read values from the sensors
  t = dht.readTemperature();
  h = dht.readHumidity();
  moistureLevel = readMoisture(moisturePin);
  brightness = photocell_calibrate();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(t) || isnan(h) ) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  } 
  else if (t == -50.00 || h == 1.00) {
    Serial.println(F("Check DHT sensor connection!!"));
    return;
  }

  // Write values to ThingSpeak
  ThingSpeak.setField(1, t); // Write temperature to field 1
  ThingSpeak.setField(2, h); // Write humidity to field 2
  ThingSpeak.setField(3, moistureLevel); // Write moisture to field 3
  ThingSpeak.setField(4, brightness);  // Write brightness to field 4
  
  // Write temperature and humidity to ThingSpeak
  ThingSpeak.writeFields(channelID, writeAPIKey);
 
  // Print values to the serial monitor
  Serial.println("Sending data to ThingSpeak...");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" °C, Humidity: ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("Moisture: ");
  Serial.print(moistureLevel);
  Serial.println("%");
  Serial.print("Brightness: ");
  Serial.print(brightness);
  Serial.println("%");

  // Publish the temperature and humidity to AWS IoT
  Serial.println("Sending data to AWS IoT...");
  publishMessage();

  // Check for any incoming messages
  client.loop();
  delay(3000);

  // Going to sleep for 30 minutes...
  Serial.println("End one cycle!");
  Serial.println("Going to sleep for 30 minutes...");
  esp_sleep_enable_timer_wakeup((int64_t)1800 * 1000000); // Wake up in 30 minutes
  esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, LOW);
  delay(100); // Wait for the serial buffer to flush
  esp_deep_sleep_start();
}