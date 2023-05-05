#include <Arduino.h>
#include <WiFi.h>

const int relayPin = 33;
const int moisturePin = 15;
const int dryValue = 3400;
const int wetValue = 2400;

int readMoisture(int moisturePin) {
  // Read the moisture level from the sensor
  int moistureLevel = analogRead(moisturePin);
  
  // Convert the analog reading to a percentage
  float percentMoisture = map(moistureLevel, dryValue, wetValue, 0, 100);
  
  // Return the moisture level as an integer
  return (int)percentMoisture;
}

void setup() {
  // Set up the relay pin
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); 
  Serial.begin(9600); 
}



void loop() {
  // Read the moisture level from the sensor
  int percentMoisture = readMoisture(moisturePin);
  Serial.println("Moisture level: " + String(percentMoisture) + "% ");
  

  // If the moisture level is below 30%, water the plant
  if (percentMoisture < 30) {
    Serial.println("Low moisture level!");
    // Water the plant until the moisture level is above 60%
    while (percentMoisture < 60) {
      Serial.print("Watering the plant... => ");
      percentMoisture = readMoisture(moisturePin);
      Serial.println("Moisture: " + String(percentMoisture) + "% ");
      digitalWrite(relayPin, HIGH); // turn on the relay
      delay(500); // wait for 0.5 seconds
      digitalWrite(relayPin, LOW); // turn off the relay
      delay(1000); // wait for 3 seconds
    }
  } else {
    Serial.println("Good moisture level!");
  }
  delay(5000); // wait for 5 seconds
}
