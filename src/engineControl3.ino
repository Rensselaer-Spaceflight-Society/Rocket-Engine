#include <string.h>
#include <time.h>
#include <stdlib.h>

//Command Statements, only add if an extra one is neeeded!
constexpr char CTRLACTV[] = "CtrlActi";
constexpr char LOG_STRT[] = "LogStart";
constexpr char INRTFLSH[] = "InertFlush";
constexpr char PRESFUEL[] = "PressFuel"
constexpr char IGN[] =  "Ignite";
constexpr char SHTDWN[] = "Shutdown";
//Pin Values
constexpr int NITROGENVALVE = 25;
constexpr int OXIDIZERVALVE = 26;
constexpr int FUELVALVE = 27;
constexpr int IGNITERVALVE = 28;
constexpr int PRESFUELVALVE = 15; //change later
//Analog Pins for Pressure Transducers on Board add "A" before number here
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


//Function Definitions
void Ignition(){
  
}
void Shutdown(){

}
void readSensor(){

}
void sendSensor(){

}
void OpenValve(int PIN){

}
void CloseValve(int PIN){

}

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
