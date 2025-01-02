#define PRODUCTION false
#define ENABLE_LOGGING false
#define ENABLE_PINGS false

#if PRODUCTION
#define BAUD_RATE 38400
#define ENABLE_LOGGING true  // Production should always have logging enabled
#define ENABLE_PINGS true    // Production should always have pings
#define SerialPort Serial1   // Use the production serial pins
#else
#define BAUD_RATE 9600
#define SerialPort Serial  // Use the USB port for debugging
#endif

#include "commands.h"
#include "pins.h"
#include "sensors.h"

char dataIn[COMMAND_SIZE_BYTES];
EngineStates currentState = EngineStates::NO_CONNECTION;
unsigned int lastPingTime = 0;
unsigned int lastEventTime = 0;
int commandRepeats = 0;

void setup() {
  SerialPort.begin(BAUD_RATE);
  while(!SerialPort){
    ;
  }
  setupPins();
  resetPins();
  lastPingTime = millis();
}

void loop() {
  // Handle any incomming commands
  if (SerialPort.available() >= COMMAND_SIZE_BYTES) {
    // SerialPort.print(SerialPort.available());
    SerialPort.readBytes(dataIn, COMMAND_SIZE_BYTES);
    // SerialPort.println(SerialPort.available());
    dataIn[8] = 0; // Null Terminator
    handleCommand(dataIn, currentState, lastPingTime, lastEventTime, commandRepeats);
  }

  // Read Sensor Data, Send it back, and validate it
  if (currentState >= EngineStates::LOG_START && ENABLE_LOGGING) {
    readSensorData(sensorData);
    sendSensorData(sensorData);
  }

  // If we should be validating data, then start validating that data
  if (currentState >= EngineStates::FUEL_PRESSURIZATION && ENABLE_LOGGING) {
    if (!nominalDataCheck(sensorData)) {
      startShutdown();
      currentState = EngineStates::SHUTDOWN_NITROGEN_FLUSH_STARTED;
      lastEventTime = millis();
      SerialPort.write(SHUTDOWN, COMMAND_SIZE_BYTES);
    }
  }

  // Handle Lost Connection
  if (commandRepeats > MAX_COMMAND_REPEATS) {
    startShutdown();
    commandRepeats = 0;
    currentState = EngineStates::CONNECTION_LOST;
    lastEventTime = millis();
    SerialPort.write(CONNECTION_LOST, COMMAND_SIZE_BYTES);
  }

  // Check for pings
  if (currentState >= EngineStates::CONNECTION_ESTABLISHED && ENABLE_PINGS) {
    int currentTime = millis();
    // If there has been too much time between pings, then attempt shutting down.
    if (currentTime - lastPingTime > MAX_PING_DELAY_MS) {
      startShutdown();
      currentState = EngineStates::CONNECTION_LOST;
      lastEventTime = millis();
      SerialPort.write(CONNECTION_LOST, COMMAND_SIZE_BYTES);
    }
  }

  // Handle any multi-step commands (inert flush, ignition, shutdown)

  // Handling finishing the pre-burn inert flush
  if (currentState == EngineStates::PRE_BURN_NITROGEN_FLUSH_STARTED) {
    int currentTime = millis();
    if ((currentTime - lastEventTime) > NITROGEN_FLUSH_DELAY_MS) {
      closeValve(NITROGENVALVE);
      currentState = EngineStates::PRE_BURN_NITROGEN_FLUSH_FINISHED;
      SerialPort.write(NITROGEN_FLUSH_FINISHED, COMMAND_SIZE_BYTES);
    }
  }

  // Handling lighting the igniter after fuel flow
  if (currentState == EngineStates::FUEL_OPEN) {
    int currentTime = millis();
    if ((currentTime - lastEventTime) > FUEL_VALVE_OPEN_DELAY_MS) {
      openValve(IGNITERVALVE);
      currentState = EngineStates::SPARK;
      lastEventTime = millis();
    }
  }

  // Handling Opening Oxidizer After Spark
  if (currentState == EngineStates::SPARK) {
    int currentTime = millis();
    if ((currentTime - lastEventTime) > SPARK_DELAY_MS) {
      openValve(OXIDIZERVALVE);
      currentState = EngineStates::IGNITION;
    }
  }

  // Handle the Shutdown Inert Flush
  if (currentState == EngineStates::SHUTDOWN_NITROGEN_FLUSH_STARTED) {
    int currentTime = millis();
    if ((currentTime - lastEventTime) > NITROGEN_FLUSH_DELAY_MS) {
      closeValve(NITROGENVALVE);
      currentState = EngineStates::SHUTDOWN_CONFIRMED;
      SerialPort.write(SHUTDOWN_CONFIRMED, COMMAND_SIZE_BYTES);  // Indicate to the control computer the shutdown is done
      digitalWrite(LED3, HIGH);
    }
  }

  // Handle the Shutdown Inert Flush when connection is lost 
  if (currentState == EngineStates::CONNECTION_LOST) {
    int currentTime = millis();
    if ((currentTime - lastEventTime) > NITROGEN_FLUSH_DELAY_MS) {
      closeValve(NITROGENVALVE);
      currentState = EngineStates::NO_CONNECTION;
      SerialPort.write(SHUTDOWN_CONFIRMED, COMMAND_SIZE_BYTES);  // Indicate to the control computer the shutdown is done
      digitalWrite(LED3, HIGH);
    }
  }
}
