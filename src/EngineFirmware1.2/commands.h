#ifndef __COMMANDS_H_
#define __COMMANDS_H_

#include "sensors.h"
#include <string.h>

#define COMMAND_SIZE_BYTES 8
#define MAX_PING_DELAY_MS 50
#define MAX_COMMAND_REPEATS 3
#define NITROGEN_FLUSH_DELAY_MS 5000
#define FUEL_VALVE_OPEN_DELAY_MS 1000
#define SPARK_DELAY_MS 500


constexpr char COMMS_ESTABLISHED[] = "CtrlActi";
constexpr char LOG_START[] = "LogStart";
constexpr char INERT_FLUSH[] = "InrtFlsh";
constexpr char PRES_FUEL[] = "PresFuel";
constexpr char IGNITION[] = "Ignition";
constexpr char SHUTDOWN[] = "Shutdown";
constexpr char PING_COMMAND[] = "PingPong";

constexpr char SHUTDOWN_CONFIRMED[] = "ShutConf";
constexpr char INVALID_COMMAND[] = "InvldCom";
constexpr char CONNECTION_LOST[] = "ConnLost";

enum class EngineStates {
  CONNECTION_LOST = -1,
  NO_CONNECTION,
  CONNECTION_ESTABLISHED,
  LOG_START,
  PRE_BURN_NITROGEN_FLUSH_STARTED,
  PRE_BURN_NITROGEN_FLUSH_FINISHED,
  FUEL_PRESSURIZATION,
  FUEL_OPEN,
  SPARK,
  IGNITION,
  SHUTDOWN_NITROGEN_FLUSH_STARTED,
  SHUTDOWN_CONFIRMED
};

char dataOut[sizeof(Sensors)];
Sensors sensorData = { 0 };

void sendSensorData(Sensors &sensorData) {
  memcpy(dataOut, &sensorData, sizeof(Sensors));
  Serial.write(dataOut, sizeof(Sensors));
}

void openValve(int pin) {
  digitalWrite(pin, HIGH);
}

void closeValve(int pin) {
  digitalWrite(pin, LOW);
}

void startShutdown() {
  resetPins();
  openValve(NITROGENVALVE);
}

void handleCommand(char * command, EngineStates &currentState, unsigned int &lastPingTime, unsigned int &lastEventTime, int &commandRepeats) {
  if (!strcmp(command, PING_COMMAND)) {
    lastPingTime = millis();
    SerialPort.write(PING_COMMAND, COMMAND_SIZE_BYTES);
    return;
  }

  if (!strcmp(command, COMMS_ESTABLISHED)) {
    // SerialPort.println("Comms Pathway");
    // If we are already in the Comms_Est state, then just reply
    if (currentState == EngineStates::CONNECTION_ESTABLISHED) {
      SerialPort.write(COMMS_ESTABLISHED, COMMAND_SIZE_BYTES);
      commandRepeats++;
      return;
    }

    //If we are in the startup (No Connection) state, then we should transition to the comms established state
    if (currentState == EngineStates::NO_CONNECTION) {
      commandRepeats = 0;
      resetPins();
      SerialPort.write(COMMS_ESTABLISHED, COMMAND_SIZE_BYTES);
      currentState = EngineStates::CONNECTION_ESTABLISHED;
      return;
    }

    // Otherwise, this indicates that comms may have been lost and we should re-start
    startShutdown();
    currentState = EngineStates::CONNECTION_LOST;
    lastEventTime = millis();
    SerialPort.write(CONNECTION_LOST, COMMAND_SIZE_BYTES);
    return;
  }

  if (!strcmp(command, LOG_START)) {
    // This is a repeat command of a command we have seen before
    if (currentState == EngineStates::LOG_START) {
      commandRepeats++;
      SerialPort.write(LOG_START, COMMAND_SIZE_BYTES);
      return;
    }

    // If we are in the Connection Established State then transition to the log start state
    if (currentState == EngineStates::CONNECTION_ESTABLISHED) {
      SerialPort.write(LOG_START, COMMAND_SIZE_BYTES);
      currentState = EngineStates::LOG_START;
      commandRepeats = 0;
      return;
    }

    // In any other state, this is an invalid command.
    SerialPort.write(INVALID_COMMAND, COMMAND_SIZE_BYTES);
    return;
  }

  if (!strcmp(command, INERT_FLUSH)) {
    if (currentState == EngineStates::PRE_BURN_NITROGEN_FLUSH_STARTED
        || currentState == EngineStates::PRE_BURN_NITROGEN_FLUSH_FINISHED) {
      commandRepeats++;
      SerialPort.write(INERT_FLUSH, COMMAND_SIZE_BYTES);
      return;
    }

    if (currentState == EngineStates::LOG_START || currentState == EngineStates::SHUTDOWN_CONFIRMED) {
      commandRepeats = 0;
      resetPins();
      SerialPort.write(INERT_FLUSH, COMMAND_SIZE_BYTES);
      openValve(NITROGENVALVE);
      lastEventTime = millis();
      currentState = EngineStates::PRE_BURN_NITROGEN_FLUSH_STARTED;
      return;
    }

    // In any other state, this is an invalid command.
    SerialPort.write(INVALID_COMMAND, COMMAND_SIZE_BYTES);
    return;
  }

  if (!strcmp(command, PRES_FUEL)) {
    if (currentState == EngineStates::FUEL_PRESSURIZATION) {
      commandRepeats++;
      SerialPort.write(PRES_FUEL, COMMAND_SIZE_BYTES);
      return;
    }

    if (currentState == EngineStates::PRE_BURN_NITROGEN_FLUSH_FINISHED) {
      commandRepeats = 0;
      SerialPort.write(PRES_FUEL, COMMAND_SIZE_BYTES);
      openValve(PRESFUELVALVE);
      currentState = EngineStates::FUEL_PRESSURIZATION;
      return;
    }

    // In any other state, this is an invalid command.
    SerialPort.write(INVALID_COMMAND, COMMAND_SIZE_BYTES);
    return;
  }

  if (!strcmp(command, IGNITION)) {
    if (currentState == EngineStates::FUEL_OPEN
        || currentState == EngineStates::SPARK
        || currentState == EngineStates::IGNITION) {
      commandRepeats++;
      SerialPort.write(IGNITION, COMMAND_SIZE_BYTES);
      return;
    }

    if (currentState == EngineStates::FUEL_PRESSURIZATION) {
      commandRepeats = 0;
      SerialPort.write(IGNITION, COMMAND_SIZE_BYTES);
      openValve(FUELVALVE);
      lastEventTime = millis();
      currentState = EngineStates::FUEL_OPEN;
      return;
    }

    // In any other state, this is an invalid command.
    SerialPort.write(INVALID_COMMAND, COMMAND_SIZE_BYTES);
    return;
  }

  if (!strcmp(command, SHUTDOWN)) {
    if (currentState == EngineStates::SHUTDOWN_NITROGEN_FLUSH_STARTED) {
      SerialPort.write(SHUTDOWN, COMMAND_SIZE_BYTES);
      return;
    }

    if (currentState == EngineStates::SHUTDOWN_CONFIRMED) {
      SerialPort.write(SHUTDOWN_CONFIRMED, COMMAND_SIZE_BYTES);
    }
    // In any other state, we must shut down immediately
    startShutdown();
    currentState = EngineStates::SHUTDOWN_NITROGEN_FLUSH_STARTED;
    lastEventTime = millis();
    SerialPort.write(SHUTDOWN, COMMAND_SIZE_BYTES);
    return;
  }

  SerialPort.write(INVALID_COMMAND, COMMAND_SIZE_BYTES);
  // Flush the input buffer if we get an unknown command
  SerialPort.end();
  SerialPort.begin(BAUD_RATE);
  while(!Serial){}

  
}


#endif  // __COMMANDS_H_