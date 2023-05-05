// Include the DHT library
#include <DHT.h>
#include <Adafruit_Sensor.h>

// Define the DHT pin
#define DHT_PIN 27

// Create a DHT object
DHT dht(DHT_PIN, DHT21);

void setup() {
  // Start the serial communication
  Serial.begin(115200);

  // Initialize the DHT sensor
  dht.begin();
}

void loop() {
  // Read the temperature and humidity values from the DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Print the temperature and humidity values to the serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" *C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Wait for 2 seconds before reading again
  delay(2000);
}
