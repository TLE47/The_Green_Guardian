// Define the relay pin
#include <Arduino.h>
#define RELAY_PIN 33

void setup() {
  // Set the relay pin as output
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
}

void loop() {
  // Turn on the relay
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("Relay on");
  delay(1000); // Wait for 1 second

  // Turn off the relay
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("Relay off");
  delay(1000); // Wait for 1 second
}
