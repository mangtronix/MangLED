
// Define your SSID and password here. Don't check in the file to source control.

// $$$ TODO - store settings e.g. in SPIFFS where it could be edited through web interface

#ifndef WIFI_SSID
#define WIFI_SSID "mled-private"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "lightitup"
#endif

// This sets the node number which corresponds to:
// - host name, e.g. mled-1
// - MIDI channel name, e.g. 1
#ifndef NODE_NUMBER
#define NODE_NUMBER 1
#endif

