# MangLED

A system to control a network of lighting nodes over WiFi, using RTP-MIDI (aka Apple Midi or rtpMIDI).

Each node:
- has an attached LED strip, that is divided into controllable "segments"
- waits for Apple Midi / rtpMIDI connections
- responds to MIDI notes and controllers
- has a number, which is also registered as an mDNS/Bonjour hostname (e.g. mled-1.local)

The MIDI mapping is configurable using #defines during the compile (in private_config.h or platformio.ini).

Default MIDI mapping:
- each node has a single segment, and listens for a single note, the first node responds to C3 (48)
  - e.g. mled-6 listens for note 53
- the brightness of the segment comes from the note velocity, the segment maximum brightness, and master brightness
- each node listens for segment brightness on controller (0 + node number)
  - e.g. mled-6 listens for brightness CC 6
- all nodes listen for master brightness on CC 40

It's also possible to make each node listen on a different MIDI channel and have more than one segment.

Default network setup:
- the nodes look for a WiFi SSID "mled-private", the password is in the code
- if you set up a WiFi router to create this network, you can connect your laptop to it via Ethernet and have Internet over your main WiFi and MangLED over the access point!
- the WiFi settings can be changed in private_config.h

Notes / Issues:
- you have to manually connect the nodes (in Audio Midi Setup.app or rtpMIDI)
- if Apple MIDI disconnects, you have to manually reconnect
- sometimes the mled-n.local hostnames take some time to appear in Audio Midi Setup
  - manually configuring a target with explicit hostname or IP allows connecting before the local hostname shows up

Build Notes:
- the easiest way to build is with PlatformIO inside Visual Studio Code
- there are helper scripts for flashing via USB and OTA you can run in the PlatformIO terminal
  - e.g. $ ./flash_node.sh 4
  - e.g. $ ./upload_to_node.sh 4
