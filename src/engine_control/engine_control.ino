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

typedef struct Sensors_Struct
{
  double loadCell;
  double pressure[6];
  double thermocouple;
} sensors;

sensors sensorData = {0};
char dataIn[16];
char dataOut[sizeof(sensors)];

void setup()
{
  Serial.begin(BAUD_RATE);

  // Wait for control room to come online
  // and inform the control room that the rocket is online
  while (strcmp(dataIn, CONTROL_ROOM_ONLINE) != 0) // Wait for the control room signal
  {
    Serial.println(ROCKET_ONLINE);                        // Indicate that the rocket is online
    if (Serial.available() >= CONTROL_ROOM_RESPONSE_SIZE) // Check if the control room has sent a signal
    {
      Serial.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE); // Read the signal
      continue;
    }
    delay(1000);
  }

  // Await for signal to start the countdown
  while (strcmp(dataIn, COUNTDOWN_START) != 0) // Wait for the countdown start signal
  {
    if (Serial.available() < CONTROL_ROOM_RESPONSE_SIZE) // Check if the control room has sent a signal
    {
      continue; // Wait for the signal
    }

    Serial.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE); // Read the signal
    if (strcmp(dataIn, ABORT_SIG) == 0)                   // Check if the control room has sent an abort signal
    {
      abortProcedure(); // Abort the procedure
    }
  }

  Serial.println(COUNTDOWN_START_REPLY);

  // Start the countdown;
  for (long i = COUNTDOWN_ITER; i >= AUTOMATIC_HOLD_ITER; i--) // Countdown from COUNTDOWN_ITER to AUTOMATIC_HOLD_ITER
  {
    if (Serial.available() < CONTROL_ROOM_RESPONSE_SIZE) // Wait for a signal from the control room
    {
      readSensors(sensorData);     // Read the sensors
      sendSensorData(&sensorData); // Send the sensor data
      continue;                    // Wait for the signal
    }

    Serial.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
    if (strcmp(dataIn, ABORT_SIG) == 0)
    {
      abortProcedure();
    }
  }

  Serial.println(AUTO_HOLD);

  while (strcmp(dataIn, HOLD_CONTINUE) != 0 && strcmp(dataIn, HOLD_STOP) != 0)
  {
    if (Serial.available() < CONTROL_ROOM_RESPONSE_SIZE) // Wait for a signal from the control room
    {
      readSensors(sensorData);     // Read the sensors
      sendSensorData(&sensorData); // Send the sensor data
      continue;                    // Wait for the signal
    }

    Serial.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE); // Read the signal
    Serial.println(dataIn);                               // Print the signal
    if (strcmp(dataIn, ABORT_SIG) == 0)                   // Check if the control room has sent an abort signal
    {
      abortProcedure(); // Abort the procedure if the control room has sent an abort signal
    }
  }

  if (strcmp(dataIn, HOLD_CONTINUE) != 0) // Check if the control room has sent a signal to continue
  {
    abortProcedure(); // Abort the procedure if the control room has sent a signal to stop
  }

  for (long i = AUTOMATIC_HOLD_ITER; i >= 0; i--)
  {
    Serial.println(i);
    if (Serial.available() < CONTROL_ROOM_RESPONSE_SIZE) // Check if the control room has sent a signal
    {
      readSensors(sensorData);
      sendSensorData(&sensorData);
      continue;
    }

    Serial.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
    if (strcmp(dataIn, ABORT_SIG) == 0)
    {
      abortProcedure();
    }
  }

  // TODO: Replace with actual ignition sequence
  Serial.println("Ignition"); 
}

void loop()
{
  if (Serial.available() >= CONTROL_ROOM_RESPONSE_SIZE) // Check if the control room has sent a signal
  {
    Serial.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE); // Read the signal
    if (strcmp(dataIn, ABORT_SIG) == 0) // Check if the control room has sent an abort signal
    {
      abortProcedure(); // Abort the procedure
    }
  }

  readSensors(sensorData);    // Read the sensors
  sendSensorData(&sensorData); // Send the sensor data
}

// Replace with proper reading
void readSensors(sensors &sensorData)
{
  sensorData.loadCell = sensorData.loadCell += 0.01;
  sensorData.thermocouple = sensorData.thermocouple += 0.01;
  sensorData.pressure[1] = sensorData.pressure[1] += 0.01;
  sensorData.pressure[2] = sensorData.pressure[2] += 0.01;
  sensorData.pressure[3] = sensorData.pressure[3] += 0.01;
  sensorData.pressure[4] = sensorData.pressure[4] += 0.01;
  sensorData.pressure[5] = sensorData.pressure[5] += 0.01;
  sensorData.pressure[6] = sensorData.pressure[6] += 0.01;
}

void abortProcedure()
{
  // Replace with proper abort procedure
  Serial.println("RCAborted");
  while (true)
  {
  }
}

void sendSensorData(sensors *sensorPtr)
{
  memcpy(&dataOut, sensorPtr, sizeof(sensorData));
  for (int i = 0; i < sizeof(sensorData); i++)
  {
    Serial.print(dataOut[i]);
  }
  Serial.println("");
}
