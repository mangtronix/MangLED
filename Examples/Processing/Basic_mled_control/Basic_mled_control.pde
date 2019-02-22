import themidibus.*; //Import the library

// Control MangLED lighting nodes from Processing!
// Shows how to set the hue and turn the nodes on/off via CC messages or notes
//
// Adapted from MidiBus example

MidiBus myBus; // The MidiBus

void setup() {
  size(400, 400);
  background(0);

  MidiBus.list(); // List all available Midi devices on STDOUT. This will show each device's index and name.
  
  // You should create a network midi session with name "mled-session", or enter the name of your network
  // session below
  myBus = new MidiBus(this, -1, "mled-session"); // Create a new MidiBus with no input device and the default Java Sound Synthesizer as the output device.
}

void draw() {
  int channel = 0;

  // Go through all the nodes and turn each one on in sequence
  int baseBrightnessCC = 0;
  int baseHueCC = 20;
  
  int brightness = 90;
  float hue = 0;
  int hueMidiValue = 0; 
  int nodeCount = 7;
  
  for (int node = 1; node <= nodeCount; node++) {
    hue = node / (float)nodeCount; // Set the hue to a value 0..1 mapped across the nodes, node rainbow!
    hueMidiValue = (int)(hue * 127); // Midi values are 0..127
    
    
    // Set the hue of the node
    println("Setting node " + node + " to hue " + hueMidiValue);
    myBus.sendControllerChange(channel, baseHueCC + node, hueMidiValue);
    
    myBus.sendControllerChange(channel, baseBrightnessCC + node, brightness);
    delay(500);
    myBus.sendControllerChange(channel, baseBrightnessCC + node, 0); // Turn off node
    delay(500);
  }

  // Turn all the nodes on with notes
  // The first node (node 1) has note 48 (C2/C3)
  int baseNote = 47;
  println("Turning on all nodes");
  for (int node = 1; node <= nodeCount; node++) {
    int pitch = baseNote + node;
    int velocity = 100;
    myBus.sendNoteOn(channel, pitch, velocity); // Send a Midi noteOn
  }
  delay(2000);
  for (int node = 1; node <= nodeCount; node++) {
    int pitch = baseNote + node;
    int velocity = 0;
    myBus.sendNoteOff(channel, pitch, velocity); // Send a Midi nodeOff
  }

  delay(2000);
  
}

void noteOn(int channel, int pitch, int velocity) {
  // Receive a noteOn
  println();
  println("Note On:");
  println("--------");
  println("Channel:"+channel);
  println("Pitch:"+pitch);
  println("Velocity:"+velocity);
}

void noteOff(int channel, int pitch, int velocity) {
  // Receive a noteOff
  println();
  println("Note Off:");
  println("--------");
  println("Channel:"+channel);
  println("Pitch:"+pitch);
  println("Velocity:"+velocity);
}

void controllerChange(int channel, int number, int value) {
  // Receive a controllerChange
  println();
  println("Controller Change:");
  println("--------");
  println("Channel:"+channel);
  println("Number:"+number);
  println("Value:"+value);
}

void delay(int time) {
  int current = millis();
  while (millis () < current+time) Thread.yield();
}
