#include <string.h>
#include <time.h>
#include <stdlib.h>

#define COUNTDOWN_ITER 30
#define AUTOMATIC_HOLD_ITER 10
#define CONTROL_ROOM_RESPONSE_SIZE 8
#define BAUD_RATE 9600
#define ROCKET_ONLINE "RCOnline"
#define CONTROL_ROOM_ONLINE "CROnline"
#define COUNTDOWN_START "CDStarts"
#define COUNTDOWN_START_REPLY "CSTARTED"
#define HOLD_CONTINUE "HoldCont"
#define HOLD_STOP "HoldStop"
#define AUTO_HOLD "AutoHold"
#define ABORT_SIG "RCAbort!"

typedef struct Sensors_Struct {
  double loadCell;
  double pressure[6];
  double thermocouple;
} sensors;


sensors sensorData = {0};
char dataIn[16];
char dataOut[sizeof(sensors)];

void readSenors(sensors& sensorData);
void abortProcedure();
void sendSensorData(sensors * sensorPtr);

void setup() {
  Serial1.begin(BAUD_RATE);

  Serial1.println(sizeof(long));

  Serial1.println(ROCKET_ONLINE);
  while (strcmp(dataIn, CONTROL_ROOM_ONLINE) != 0) {
    if (Serial1.available() >= CONTROL_ROOM_RESPONSE_SIZE) {
      Serial1.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
      Serial1.println(dataIn);
      continue;
    }
    Serial1.println(ROCKET_ONLINE);
    delay(1000);
  }


  // Await for signal to start the countdown
  while (strcmp(dataIn, COUNTDOWN_START) != 0) {
    if (Serial1.available() >= CONTROL_ROOM_RESPONSE_SIZE) {
      Serial1.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
      Serial1.println(dataIn);
      if (strcmp(dataIn, ABORT_SIG) == 0) {
        abortProcedure();
      }
      continue;
    }
  }

  Serial1.println(COUNTDOWN_START_REPLY);

  // Start the countdown;
  for (long i = COUNTDOWN_ITER; i >= AUTOMATIC_HOLD_ITER; i--) {
    Serial1.println(i);
    if (Serial1.available() >= CONTROL_ROOM_RESPONSE_SIZE) {
      Serial1.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
      if (strcmp(dataIn, ABORT_SIG) == 0) {
        abortProcedure();
      }
    }
    readSensors(sensorData);
    sendSensorData(&sensorData);
  }

  Serial1.println(AUTO_HOLD);

  while (strcmp(dataIn, HOLD_CONTINUE) != 0 && strcmp(dataIn, HOLD_STOP) != 0) {
    if (Serial1.available() >= CONTROL_ROOM_RESPONSE_SIZE) {
      Serial1.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
      Serial1.println(dataIn);
      if (strcmp(dataIn, ABORT_SIG) == 0) {
        abortProcedure();
      }
    }
    // readSensors(sensorData);
    // sendSensorData(&sensorData);
  }

  if (strcmp(dataIn, HOLD_CONTINUE) != 0) {
    abortProcedure();
  }

  for (long i = AUTOMATIC_HOLD_ITER; i >= 0; i--) {
    if (Serial1.available() >= CONTROL_ROOM_RESPONSE_SIZE) {
      Serial1.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
      if (strcmp(dataIn, ABORT_SIG) == 0) {
        abortProcedure();
      }
    }
    readSensors(sensorData);
    sendSensorData(&sensorData);
    Serial1.println(i);
  }

  Serial1.println("Ignition");
}


void loop() {
  if (Serial1.available() >= CONTROL_ROOM_RESPONSE_SIZE) {
    Serial1.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
    if (strcmp(dataIn, ABORT_SIG) == 0) {
      abortProcedure();
    }
  }
  readSensors(sensorData);
  sendSensorData(&sensorData);
}

// Replace with proper reading
void readSensors(sensors& sensorData) {
  sensorData.loadCell = sensorData.loadCell += 0.01;
  sensorData.thermocouple = sensorData.thermocouple += 0.01;
  sensorData.pressure[1] = sensorData.pressure[1] += 0.01;
  sensorData.pressure[2] = sensorData.pressure[2] += 0.01;
  sensorData.pressure[3] = sensorData.pressure[3] += 0.01;
  sensorData.pressure[4] = sensorData.pressure[4] += 0.01;
  sensorData.pressure[5] = sensorData.pressure[5] += 0.01;
  sensorData.pressure[6] = sensorData.pressure[6] += 0.01;
}

void abortProcedure() {
  // Replace with proper abort procedure
  Serial1.println("RCAborted");
  while (true) {}
}

void sendSensorData(sensors* sensorPtr){
  memcpy(&dataOut, sensorPtr, sizeof(sensorData));
  for(int i = 0; i < sizeof(sensorData); i++){
    Serial1.print(dataOut[i]);
  }
  Serial1.println("");
}
