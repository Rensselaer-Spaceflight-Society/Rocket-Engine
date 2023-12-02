#include<SPI.h>
#include<RF24.h>
#include<nRF24L01.h>

#define CE 8
#define CSN 53

RF24 radio(CE, CSN);
const byte address[5] = "0001";

void setup(){
  Serial.begin(9600);
  if(!radio.begin()){
    Serial.println("Failed To Start Radio");
    while(1){}
  }
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop(){
  if(radio.available()){
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
  }else{
    Serial.println("Radio Unavailable");
  }
}