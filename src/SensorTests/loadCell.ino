#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 24;
const int LOADCELL_SCK_PIN = 22;

HX711 scale;

long numReads = 5;

void setup() {
  Serial.begin(38400);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}

void loop() {

  if (scale.is_ready()) {
    long reading = 0;
    for(int i = 0; i < numReads;i++){
      reading += scale.read();
    }
    Serial.println("Reading: ");
    Serial.println(((float) ((reading/numReads) + 2383.0)/904.0) - 10 +'\n');
  } else {
    Serial.println("HX711 not found.");
  }

  delay(100);
  
}
