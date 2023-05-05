#include <Arduino.h>
// Define the photocell pin
#define PHOTCELL_PIN 37
int photocellValue = 0;

void setup() {
  // Start the serial communication
  Serial.begin(9600);

  // Set the photocell pin as input
  pinMode(PHOTCELL_PIN, INPUT);
}


void photocell_calibrate() {
  int total = 0
  for (int i = 0; i < 100; i++) {
    photocellValue = analogRead(PHOTCELL_PIN);
    total += photocellValue;
    delay(100);
  }
  photocellValue = total / 100;
  Serial.println(photocellValue);
}

void loop() {
  photocell_calibrate();

  // Print the analog value to the serial monitor
  Serial.print("Photocell value: ");
  Serial.println(photocellValue);

  // Wait for 3 second before reading again
  delay(3000);
}
