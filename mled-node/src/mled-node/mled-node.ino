#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>


// Define your wifi SSID / pass in the external header file
#include "private_config.h"

const char* gSSID = WIFI_SSID;
const char* gPassword = WIFI_PASSWORD;
const int gNodeNumber = NODE_NUMBER;

String gNodeName;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // Connect to wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(gSSID, gPassword);
  Serial.print("Connecting to SSID: ");
  Serial.println(gSSID);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to SSID ");
  Serial.println(gSSID);
  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Set up our .local hostname
  gNodeName = String("mled-") + String(NODE_NUMBER);

  // Let background processing happen
  yield();

  // Set up our mled-n.local hostname
  setupBonjour();


  Serial.println("Finished setup, here we go!");
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  Serial.print(".");

}


void setupBonjour() {
   // Set up mDNS responder:
  Serial.println("Setting up mDNS / Bonjour with hostname / IP:");
  Serial.print(gNodeName);
  Serial.print(".local ");
  Serial.println(WiFi.localIP());
  if (!MDNS.begin(gNodeName.c_str())) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      Serial.print(".");
      delay(1000);
      // XXX break out if not done after awhile
      // XXX what happens if name is already registered by another node?
    }
  }
  Serial.println("mDNS responder started");
}
