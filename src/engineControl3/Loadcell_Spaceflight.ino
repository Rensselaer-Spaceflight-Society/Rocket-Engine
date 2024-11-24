#include "HX711.h"

#define DT_PIN 24  // Connect to HX711 DT
#define SCK_PIN 22 // Connect to HX711 SCK

HX711 scale;

void setup() {
  Serial.begin(9600);
  scale.begin(DT_PIN, SCK_PIN);
  
  Serial.println("Initializing...");
  
  // Optional: Set scale calibration factor (experimentally determined)
  scale.set_scale(1);  // Adjust this based on your load cell
  scale.tare();  // Reset the scale to 0
}

void loop() {
  if (scale.is_ready()) {
    long reading = scale.get_units(1); // Average over 10 readings
    Serial.print("Weight: ");
    Serial.println(reading);
  } else {
    Serial.println("HX711 not found.");
  }
  
  delay(500);
}
