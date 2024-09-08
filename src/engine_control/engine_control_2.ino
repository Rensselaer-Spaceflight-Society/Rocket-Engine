#include <string.h>
#include <time.h>
#include <stdlib.h>

constexpr int CONTROL_ROOM_RESPONSE_SIZE = 6;
constexpr int BAUD_RATE = 9600;

// constexpr char ON_READY[] = "Turn_On_Ready";
constexpr char CTRL_ACTI[] = "CtrlActi";
constexpr char INERT_GAS[] = "InertGas";
constexpr char LOG_STR[] = "LogStart";
constexpr char INERT_STOP[] = "InertStp";
constexpr char FUEL_START[] = "FuelStt";
constexpr char OXIDIZER_START[] = "OxidStt";
constexpr char IGN[] = "Ignition";


constexpr char TEST_ACTI[] = "TestActi";
constexpr char INERT_FLW[] = "InertFlw"; //Inert Gas Flowing
constexpr char SH_DWN[] = "Shutdown";
constexpr char ST_LOG[] = "StopLogs";
constexpr char ABORT_SIG[] = "AbortStp"; //Terminate
constexpr char FUEL_FLOW[] = "FuelFlow"; //Fuel Flowing

constexpr int NITROGEN = 25;
constexpr int OXIDIZER = 26;
constexpr int FUEL = 27;
constexpr int IGNITER = 28;

char dataIn[16];  //Using a regular C-style array
String incoming_Signal;

typedef struct {
  float loadCell;
  float thermocouple;
  float pressure[6];
} Sensors;
Sensors sensorData = {0};

// void control() {
//   Serial.println("Type ON_READY to begin the setup");
//   String receiveInput = "";
//   while (true) {
//     if (Serial.available > 0) {
//       char incomingChar = Serial.read(); //Read one byte(letter) at a time
//       receiveInput += incoming_Char; //Append to the input String
//       if (receiveInput == ON_READY) { //Check if the input matches "Turn_On_Ready"
//         Serial.println("Computer is now starting...");
//         break;
//       }
//     }
//   }
// }

void sendSensorData(const Sensors &sensorData) { // Sending sensor data in a structured format (32 bytes)
  Serial.write((const uint8_t*)(&sensorData), sizeof(Sensors));
}
        
void setup() {
  Serial.begin(BAUD_RATE);  //Start Serial1 for communication with Control Room
  
  //Add a temporarily valve pin (for later use)
  pinMode(NITROGEN, OUTPUT);
  pinMode(OXIDIZER, OUTPUT);
  pinMode(FUEL, OUTPUT);
  pinMode(IGNITER, OUTPUT);

  //Send TestActi until we receive CtrlActi
  while (incoming_Signal != CTRL_ACTI) {
    if (Serial.available() >= CONTROL_ROOM_RESPONSE_SIZE) {
      Serial.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);  //Read incoming signal into dataIn array
      incoming_Signal = String(dataIn);  //Verify if incoming_Signal = "CtrlActi"
      continue;
      }
      Serial.print(TEST_ACTI);  //Send TestActi (if not CtrlActi)
      delay(1000);
    }
  
  Serial.println("CommsEst"); //"Communications Established"

  // Now we want to wait for the control room to start logging data
  while(incoming_Signal != LOG_STR) {
    if (Serial.available() >= CONTROL_ROOM_RESPONSE_SIZE) {
      Serial.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
      incoming_Signal = String(dataIn);
      continue;
    }
  }

  sendSensorData(sensorData);

  // Now we want to wait for the control room to start the inert gas flush
  while(incoming_Signal != INERT_GAS) {
    if (Serial.available() >= CONTROL_ROOM_RESPONSE_SIZE) {
      Serial.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
      incoming_Signal = String(dataIn);
      continue;
    }
  }

  Serial.println(INERT_FLW); //Inert Gas Flowing
  OpenInertGasValves();

  // Now we want to wait for the control room to stop the inert gas flush
  while(incoming_Signal != INERT_STOP) {
    if (Serial.available() >= CONTROL_ROOM_RESPONSE_SIZE) {
      Serial.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
      incoming_Signal = String(dataIn);
      continue;
    }
  }

  Serial.println(INERT_STOP); //Inert Gas Stopped
  CloseInertGasValves();

  // Now we want to wait for the control room to start the ignition
  while (incoming_Signal != IGN) {
    if (Serial.available() >= CONTROL_ROOM_RESPONSE_SIZE) {
      Serial.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
      incoming_Signal = String(dataIn);
      continue;
    }
  }

  Serial.println(IGN); //Ignition

  

}

//Start collecting data
void readSensors(sensors &sensorData) { // Temporarily data (engine_control.ino)
  sensorData.loadCell += 0.01;
  sensorData.thermocouple += 0.01;
  sensorData.pressure[1] += 0.01;
  sensorData.pressure[2] += 0.01;
  sensorData.pressure[3] += 0.01;
  sensorData.pressure[4] += 0.01;
  sensorData.pressure[5] += 0.01;
  sensorData.pressure[6] += 0.01;
}
void sendSensorData(const sensors &sensorData) {
  
  // TODO: Better handling if write buffer is full
  readSensors(sensorData);
  Serial.write(sensorData, 32);

void StopLogs() {
  if (incoming_Signal == ST_LOG) {
    Serial.println("Terminating Data Logging..."); //Stop reading sensors and sending data
    while (true) {
      if (Serial.available() >= CONTROL_ROOM_RESPONSE_SIZE) {
        Serial.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
        incoming_Signal = String(dataIn);
        if (incoming_Signal == SH_DWN) {
          Serial.println("Shutdown initiated after StopLogs...");
          break;
        }
      }
    }
  }
}
  
}
void LogStart() {
  
}

void InertGas() {
  if (incoming_Signal == INERT_FLW) {
    Serial.println("Inert gas flush starts");
    //Open valves for inert gas flush
    digitalWrite(NITROGEN, HIGH);

    while (true) {
      readSensors(sensorData);
      sendSensorData(sensorData);
      if (Serial.available() >= CONTROL_ROOM_RESPONSE_SIZE) {
        Serial.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
        incoming_Signal = String(dataIn);
        if (incoming_Signal == ABORT_SIG) {
          Serial.println("Inert gas flush stops");
          //Close the valves
          digitalWrite(NITROGEN, LOW);
          break;
        }
      }
      delay(1000);
    }
  } else {
    Serial.println("Invalid Signal. Waiting for InertFlw...")
  }
}

void Ignition() {
  if (incoming_Signal == IGN) {
    Serial.println("Ignition starts");
    // Open oxidizer valves for ignition
    digitalWrite(oxidizer_valves1, HIGH);
    digitalWrite(oxidizer_valves2, HIGH);
    digitalWrite(oxidizer_valves3, HIGH);
    // Open fuel valves for ignition
    digitalWrite(fuel_valves1, HIGH);
    digitalWrite(fuel_valves2, HIGH);
    digitalWrite(fuel_valves3, HIGH);
    while (true) {
      readSensors(sensorData);
      sendSensorData(sensorData);
      if (Serial.available() >= CONTROL_ROOM_RESPONSE_SIZE) {
        Serial.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
        incoming_Signal = String(dataIn);
        if (incoming_Signal == ABORT_SIG) {
          Serial.println("Ignition Stops");
          // Close oxidizer valves
          digitalWrite(oxidizer_valves1, LOW);
          digitalWrite(oxidizer_valves2, LOW);
          digitalWrite(oxidizer_valves3, LOW);
          // Close fuel valves
          digitalWrite(fuel_valves1, LOW);
          digitalWrite(fuel_valves2, LOW);
          digitalWrite(fuel_valves3, LOW);
          break;
        }
      }
      delay(1000);
    }
  } else {
    Serial.println("Invalid Signal. Waiting for Ignition...");
  }
}

void Shutdown() {
  if (incoming_String == SH_DWN) {
    Serial.println("Shutdown");
    // Close oxidizer valves
    digitalWrite(oxidizer_valves1, LOW);
    digitalWrite(oxidizer_valves2, LOW);
    digitalWrite(oxidizer_valves3, LOW);
    // Close fuel valves
    digitalWrite(fuel_valves1, LOW);
    digitalWrite(fuel_valves2, LOW);
    digitalWrite(fuel_valves3, LOW);
    //Perform another inert gas flush (for safety)
    Serial.println("Inert gas flush starts");
    digitalWrite(valve1, HIGH);
    digitalWrite(valve2, HIGH);
    digitalWrite(valve3, HIGH);
    delay(5000); //Keep the inert gas flush for 5 seconds
    //Close the inert gas flush after
    digitalWrite(valve1, LOW);
    digitalWrite(valve2, LOW);
    digitalWrite(valve3, LOW);
    
    Serial.println("Inert gas flush stops. Shutdown complete.");
  }
}
