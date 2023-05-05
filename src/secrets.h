#include <pgmspace.h>
 
#define SECRET
#define THINGNAME "ESP32_TGG"               //change this

const char WIFI_SSID[] = "";               //change this
const char WIFI_PASSWORD[] = "";           //change this
const char AWS_IOT_ENDPOINT[] = "";       //AWS_IOT/Settings/Endpoint
 
// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
// Hidden for privacy
-----END CERTIFICATE-----



)EOF";
 
// Device Certificate                                               //change this
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
// Hidden for privacy
-----END CERTIFICATE-----


 
 
)KEY";
 
// Device Private Key                                               //change this
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
// Hidden for privacy
-----END RSA PRIVATE KEY-----




)KEY";