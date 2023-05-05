#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>

#define DHTPIN 27 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT21 // DHT 21 (AM2301)

// Your WiFi network credentials
const char* ssid = "iPhone";
const char* password = "87654321";

// Your ThingSpeak channel details
unsigned long channelID = 2130323; // Change this to your channel ID
const char* writeAPIKey = "8RGA25BI3RXD8KKK"; // Change this to your write API key

// Initialize DHT sensor
DHT dht(DHTPIN, DHT21);
WiFiClient client;

void setup() {
  // Initialize serial port
  Serial.begin(9600);

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  // Start the DHT sensor
  dht.begin();
}

void loop() {
  // Read temperature and humidity from DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if any reading failed
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  
  // Write temperature and humidity to ThingSpeak
  ThingSpeak.writeFields(channelID, writeAPIKey);

  // Print temperature and humidity to serial port
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" degrees Celcius");
  Serial.print("  Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Wait 15 seconds before next reading
  delay(15000);
}