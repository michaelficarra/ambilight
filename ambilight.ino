#include "SPI.h"
#include "WS2801.h"

#define dataPin 2
#define clockPin 3

// the number of LED/WS2801 pairs; must be in the range [1, 2**8)
#define stripLength 25

// specified under `rate` in the `[device]` section of /etc/boblight.conf
#define serialRate 38400

// boblightd sends a prefix (defined in /etc/boblight.conf) before sending the pixel data
uint8_t prefix[] = {0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA};

WS2801 strip = WS2801(stripLength, dataPin, clockPin);

void setup() {
  // initialise our LED strip
  strip.begin();
  // make the first pixel red as an indicator that it is awaiting data
  strip.setPixelColor(0, 10, 0, 0);
  // turn all the other pixels on so we know they're working
  for(uint16_t i = 1; i < stripLength; ++i)
    strip.setPixelColor(i, 5, 5, 5);
  strip.show();
  Serial.begin(serialRate);
}

void loop() {
  // wait until we see the prefix
  for(byte i = 0; i < sizeof prefix; ++i) {
    waitLoop: while (!Serial.available()) ;;
    // look for the next byte in the sequence if we see the one we want
    if(prefix[i] == Serial.read()) continue;
    // otherwise, start over
    i = 0;
    goto waitLoop;
  }
  // read the transmitted data
  for (uint8_t i = 0; i < stripLength; i++) {
    byte r, g, b;
    while(!Serial.available());
    r = Serial.read();
    while(!Serial.available());
    g = Serial.read();
    while(!Serial.available());
    b = Serial.read();
    strip.setPixelColor(i, r, g, b);
  }
  strip.show();
}
