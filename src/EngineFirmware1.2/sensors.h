
#ifndef __SENSORS_H_
#define __SENSORS_H_

#include "pins.h"
#include "HX711.h"

constexpr float voltageToTempScale = 0.005;
constexpr float PSItoKPaFactor = 6.89476;
constexpr char HEADER[] = { 'D', 'a', 't', 'a', 'P', 'a', 'c', 'k' };

typedef struct sensors_struct {
  char header[8];
  float loadCell;
  float thermocouple[4];
  float pressure[6];
  char checkSum[12];
} Sensors;

float processThermocoupleValue(int analogSignal) {
  float voltage = analogSignal * (5.0 / 1023.0);
  return voltage / voltageToTempScale;  // Convert to Celsius
}

float read_loadcell(HX711& scale){
   if (scale.is_ready()){
     return ((float) (scale.read() +2383.0)/904.0 - 10);
  }
   else{
     return 0;
   }
}

float processPressureValue(int analogSignal) {
  // 0.5v -> 0 PSI
  // 4.5v -> 500 PSI
  float voltage = analogSignal * (5.0 / 1023.0);
  float psi = (voltage - 0.5) * (500 / 4.0);  // Convert to PSI
  return psi * PSItoKPaFactor;                // Convert to KPa
}

void checksum12(void *checksum, const void *data, int n) {
  char *checksumPtr = (char *)checksum;
  const char *dataPtr = (char *)data;
  memset(checksumPtr, 0, 12);
  for (int i = 0; i < n; ++i) {
    checksumPtr[i % 12] ^= dataPtr[i];
    checksumPtr[(i + 1) % 12] ^= (dataPtr[i] >> 4) | (dataPtr[i] << 4);
  }
}

void readSensorData(Sensors &sensorData) {
  // Copy the data header to the struct
  memcpy(&sensorData.header, HEADER, 8);

  // TODO: Handle reading from the Load Cell
  sensorData.loadCell = read_loadcell(scale);

  sensorData.pressure[0] = processPressureValue(analogRead(PRESSSEN1));
  sensorData.pressure[1] = processPressureValue(analogRead(PRESSSEN2));
  sensorData.pressure[2] = processPressureValue(analogRead(PRESSSEN3));
  sensorData.pressure[3] = processPressureValue(analogRead(PRESSSEN4));
  sensorData.pressure[4] = processPressureValue(analogRead(PRESSSEN5));
  sensorData.pressure[5] = processPressureValue(analogRead(PRESSSEN6));

  sensorData.thermocouple[0] = processThermocoupleValue(analogRead(THERMOCPL1));
  sensorData.thermocouple[1] = processThermocoupleValue(analogRead(THERMOCPL2));
  sensorData.thermocouple[2] = processThermocoupleValue(analogRead(THERMOCPL3));
  sensorData.thermocouple[3] = processThermocoupleValue(analogRead(THERMOCPL4));

  // Compute the checksum for the sensor data
  checksum12(&sensorData.checkSum, &sensorData, 52);
}


// TODO: Implement
bool nominalDataCheck(Sensors &sensorData) {
  return true;
}


#endif
