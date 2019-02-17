#ifndef _PRIVATE_CONFIG_H
#define _PRIVATE_CONFIG_H

// Define your SSID and password here. Don't check in the file to source control.
// It's also possible to define these in the platformio.ini file or terminal

// $$$ TODO - store settings e.g. in SPIFFS where it could be edited through web interface

#ifndef WIFI_SSID
#define WIFI_SSID "mled-private"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "lightitup"
#endif

// To set a new password, get the hash and put it here. This way the plaintext
// password is not stored on the device or in this file.
// $ echo "my password" | md5
//
// The default password is "superinsecure"
#ifndef OTA_PASSWORD_HASH
#define OTA_PASSWORD_HASH "ed25ab87f72c8a72a73e2b492fbe3cd1"
#endif

// This sets the node number which corresponds to:
// - host name, e.g. mled-1
// - MIDI channel name, e.g. 1
#ifndef NODE_NUMBER
#define NODE_NUMBER 1
#endif

// The MIDI channel to respond to. Can be <= 0 to respond on all channels (omni)
#ifndef MLED_CHANNEL
//#define MLED_CHANNEL NODE_NUMBER
#define MLED_CHANNEL -1
#endif

// The first note to respond to. Defaults to GM Drum kit, starts at B0
#ifndef MLED_BASE_NOTE
#define MLED_BASE_NOTE 35
#endif 

// The first controller for hue to respond to
#ifndef MLED_BASE_HUE_CC
#define MLED_BASE_HUE_CC 1
#endif

// The first controller for brightness multiplier (max brightness) to respond to
#ifndef MLED_BASE_BRIGHTNESS_CC
#define MLED_BASE_BRIGHTNESS_CC 11
#endif

// The master brightness controller
#ifndef MLED_MASTER_BRIGHTNESS_CC
#define MLED_MASTER_BRIGHTNESS_CC 9
#endif



// Physical LED strip defines
// It's possible to define these separately for the different nodes by editing
// the build_flags in platformio.ini or doing a manual #define in your source file

#ifndef LED_PIN
#define LED_PIN D3
#endif

#ifndef LED_COUNT
#define LED_COUNT 7
#endif

#ifndef LED_TYPE
#define LED_TYPE WS2812
#endif

#ifndef LED_COLOR_ORDER
#define LED_COLOR_ORDER GRB
#endif



#endif