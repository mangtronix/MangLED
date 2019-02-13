#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>


// Define your wifi SSID / pass in the external header file
#include "wifi_private.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(115200);

  // Connect to wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to SSID: ");
  Serial.println(ssid);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to SSID ");
  Serial.println(ssid);
  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  Serial.print(".");

}
