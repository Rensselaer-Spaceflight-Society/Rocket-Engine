#include <string.h>
#include <time.h>
#include <stdlib.h>

//Command Statements, only add if an extra one is neeeded!
constexpr char CTRLACTV[] = "CtrlActi";
constexpr char LOG_STRT[] = "LogStart";
constexpr char INRTFLSH[] = "InertFlush";
constexpr char PRESFUEL[] = "PressFuel";
constexpr char IGN[] =  "Ignite";
constexpr char SHTDWN[] = "Shutdown";
//Pin Values FOR VALVES ALL DIGITAL
constexpr int NITROGENVALVE = 25;
constexpr int OXIDIZERVALVE = 26;
constexpr int FUELVALVE = 27;
constexpr int IGNITERVALVE = 28;
constexpr int PRESFUELVALVE = 15; //change later. AHFKHASKHASKDHASKDJHASKDJHASD:KHASDPJHAD:
//Analog Pins for Pressure Transducers on Board add "A" before number here CHANGE NAMES LATER LMAO
constexpr int PRESSSEN1 = 0;
constexpr int PRESSSEN2 = 1;
constexpr int PRESSSEN3 = 2;
constexpr int PRESSSEN4 = 3;
constexpr int PRESSSEN5 = 4;
constexpr int PRESSSEN6 = 5;
//Digital Pin for Load Cell, on Board add "D" before number here
constexpr int LOADCELL = 24;
//Digital Pin for Thermocouples CHIP SELECT add "D" before number here, only one can be set to HIGH at once
constexpr int THERMOCPL1CS = 22;
constexpr int THERMOCPL2CS = 29;
constexpr int THERMOCPL3CS = 30;
constexpr int THERMOCPL4CS = 31;
//Sensor Struct, data in/out
char dataIn[16];
typedef struct {
  float loadCell;
  float thermocouple[4];
  float pressure[6];
} Sensors;
Sensors sensorData = {0};
char dataOut[sizeof(Sensors)];
int MTIME = 0;

void maintainComms(){

}
void NitrogenFlush(int length){
  OpenValve(NITROGENVALVE);
  waitTime(length);
  CloseValve(NITROGENVALVE);
}
void Ignition(){
  NitrogenFlush(10);
  OpenValve(FUELVALVE);
  waitTime(1000);
  OpenValve(IGNITERVALVE);
  OpenValve(OXIDIZERVALVE);
}

void Shutdown(){
  CloseValve(OXIDIZERVALVE);
  waitTime(1);
  CloseValve(FUELVALVE);
  waitTime(1000);
  NitrogenFlush(2);
  waitTime(500);
  OpenValve(OXIDIZERVALVE);
  waitTime(500);
  OpenValve(NITROGENVALVE);
}

void readSensor(Sensors &sensorData){
  sensorData.loadCell = digitalRead(LOADCELL);
  sensorData.pressure[1] = analogRead(PRESSSEN1);
  sensorData.pressure[2] = analogRead(PRESSSEN2);
  sensorData.pressure[3] = analogRead(PRESSSEN3);
  sensorData.pressure[4] = analogRead(PRESSSEN4);
  sensorData.pressure[5] = analogRead(PRESSSEN5);
  sensorData.pressure[6] = analogRead(PRESSSEN6);
  //Change LATEERERERER
  sensorData.thermocouple[1] = digitalRead(THERMOCPL1CS); 
  sensorData.thermocouple[2] = digitalRead(THERMOCPL2CS); 
  sensorData.thermocouple[3] = digitalRead(THERMOCPL3CS);
  sensorData.thermocouple[4] = digitalRead(THERMOCPL4CS);
}
void sendSensor(Sensors &sensorData){
  readSensor(sensorData);
  memcpy(dataOut, &sensorData, sizeof(Sensors));
  Serial.write(dataOut, sizeof(Sensors));
}
void OpenValve(int PIN){
  digitalWrite(PIN, HIGH);
}
void CloseValve(int PIN){
  digitalWrite(PIN, LOW);
}
void waitTime(int milliseconds){
  while(millis() < (MTIME + milliseconds)){
    sendSensor(sensorData); 
  }
}
void setup() {
  // PIN SETUP FOR SENSORS
  pinMode(LOADCELL, INPUT);
  pinMode(PRESSSEN1, INPUT);
  pinMode(PRESSSEN2, INPUT);
  pinMode(PRESSSEN3, INPUT);
  pinMode(PRESSSEN4, INPUT);
  pinMode(PRESSSEN5, INPUT);
  pinMode(PRESSSEN6, INPUT);
  pinMode(THERMOCPL1CS, INPUT);
  pinMode(THERMOCPL2CS, INPUT);
  pinMode(THERMOCPL3CS, INPUT);
  pinMode(THERMOCPL4CS, INPUT);
  //Valve PIN SETUP
  pinMode(NITROGENVALVE, OUTPUT);
  pinMode(OXIDIZERVALVE, OUTPUT);
  pinMode(FUELVALVE, OUTPUT);
  pinMode(IGNITERVALVE, OUTPUT);
  pinMode(PRESFUELVALVE, OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  MTIME = millis();
}
