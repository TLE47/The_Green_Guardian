#include <Arduino.h>

const int soilSensorPin = 37;

int dryValue = 250;
int wetValue = 900;
int friendlyDryValue = 0;
int friendlyWetValue = 100;
int percentageValue = 0;
int rawValue = 0;

int moisture_calibrate() {
  rawValue = analogRead(soilSensorPin);
  
  percentageValue = map(rawValue, dryValue, wetValue, friendlyDryValue, friendlyWetValue);
  return percentageValue;
}

void setup() {
  Serial.begin(115200);
  // Calibrate the sensor
  // for (int i = 0; i < 10; i++) {
  //   int rawValue = analogRead(soilSensorPin);
  //   dryValue = min(dryValue, rawValue);
  //   delay(100);
  // }
  // Serial.println(dryValue);
  // for (int i = 0; i < 100; i++) {
  //   int rawValue = analogRead(soilSensorPin);
  //   wetValue = max(wetValue, rawValue);
  //   delay(100);
  // }
  // Serial.println(wetValue);
}
void loop() {
  
  // Serial.print("Raw: ");
  // Serial.print(rawValue);
  // Serial.print(" | ");
  
  // int friendlyValue = map(rawValue, dryValue, wetValue, friendlyDryValue, friendlyWetValue);
  
  // Serial.print("Friendly: ");
  // Serial.print(friendlyValue);
  // Serial.println("%");
  Serial.println(moisture_calibrate());
  
  delay(500);
}