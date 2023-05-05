#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "iPhone";
const char* password = "87654321";

void get_weather() {
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/forecast?q=San%20Diego&appid=""&units=metric"; // Replace " " (include quotes) to your APIkey
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
      const char* status = obj["weather"][0]["main"];

      Serial.print("Temperature: ");
      Serial.println(temp);
      Serial.print("Humidity: ");
      Serial.println(humidity);
      Serial.print("Status: ");
      Serial.println(status);
      Serial.println();
    }
  }

  http.end();
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  delay(1000); 
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");

  get_weather();
}

void loop() {
}