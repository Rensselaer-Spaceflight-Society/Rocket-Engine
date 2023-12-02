#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

#define CE 8
#define CSN 53

RF24 radio(CE, CSN); // CE CSN
const uint8_t address[5] = "0001"; 

void setup(){
  Serial.begin(9600);
  Serial.println("Starting UP");
  if (!radio.begin()) 
  { 
    Serial.println(F("radio hardware not responding!"));
    while (1) {} // hold program in infinite loop 
  }
  radio.openWritingPipe(&address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  radio.setAutoAck(false);
}

void loop(){
  const char txt[] = "SPACEFLIGHT";
  if(radio.write(&txt, sizeof(txt))){
    Serial.println("Sent");
  }else{
    Serial.println("Failed to Send.");
  }
  delay(500);
}