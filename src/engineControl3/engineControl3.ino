#include <string.h>
#include <time.h>
#include <stdlib.h>

// Constants
constexpr int BAUD_RATE = 9600;
constexpr int CONTROL_ROOM_RESPONSE_SIZE = 8;

// Commands
constexpr char CTRLACTV[] = "CtrlActi";
constexpr char LOG_STRT[] = "LogStart";
constexpr char INRTFLSH[] = "InrtFlsh";
constexpr char PRESFUEL[] = "PresFuel";
constexpr char IGN[] = "Ignition";
constexpr char SHTDWN[] = "Shutdown";
constexpr char PNG[] = "PingPong";
constexpr char HEADER[] = {'D', 'a', 't', 'a', 'P', 'a', 'c', 'k'};

// Pin Assignments
constexpr int NITROGENVALVE = 25;
constexpr int PRESFUELVALVE = 26;
constexpr int FUELVALVE = 27;
<<<<<<< HEAD
constexpr int OXIDIZERVALVE = 28;
constexpr int IGNITERVALVE = 29;

// Analog Pins for Pressure Transducers
=======
constexpr int IGNITERVALVE = 28;
constexpr int PRESFUELVALVE = 15;
>>>>>>> 7c94f59b65b66a92612126d31881f3e22a1934c8
constexpr int PRESSSEN1 = 0;
constexpr int PRESSSEN2 = 1;
constexpr int PRESSSEN3 = 2;
constexpr int PRESSSEN4 = 3;
constexpr int PRESSSEN5 = 4;
constexpr int PRESSSEN6 = 5;
constexpr int LOADCELL = 24;
constexpr int THERMOCPL1 = 6;
constexpr int THERMOCPL2 = 7;
constexpr int THERMOCPL3 = 8;
constexpr int THERMOCPL4 = 9;
<<<<<<< HEAD

// LED Ports (Digital)
constexpr int LED1 = 30;
constexpr int LED2 = 31;
constexpr int LED3 = 32;
=======
constexpr int LED1 = 5;
constexpr int LED2 = 6;
constexpr int LED3 = 7;
>>>>>>> 7c94f59b65b66a92612126d31881f3e22a1934c8

// Constants for calculations
constexpr float voltageToTempScale = 0.005;

// Sensor Data Structure
struct Sensors {
    char header[8];
    float loadCell;
    float thermocouple[4];
    float pressure[6];
    char checkSum[12];
};

Sensors sensorData = {0};
char dataIn[16];
char dataOut[sizeof(Sensors)];
String incomingSignal;

int masterTime = 0;
int timeSinceLastPing = 0;
int lastPingTime = 0;
bool isBurning = false;

// Helper Functions
void openValve(int pin) {
    digitalWrite(pin, HIGH);
}

void closeValve(int pin) {
    digitalWrite(pin, LOW);
}

float processThermoCoupleValue(int analogSignal) {
    float voltage = analogSignal * (5.0 / 1023.0);
    return voltage / voltageToTempScale; // Convert to Celsius
}

float processPressureValue(int analogSignal) {
    float voltage = analogSignal * (5.0 / 1023.0);
    return (voltage - 0.5) * (500 / 4.0); // Convert to PSI
}

void waitTime(int milliseconds) {
    masterTime = millis();
    while (millis() < (masterTime + milliseconds)) {
        sendSensor(sensorData); // Continuously send sensor data
    }
}

// Communication and Data Handling
String checkData(const char* validCommands[], size_t commandCount) {
    static String incomingSignal = "";

    while (Serial.available() > 0) {
        char receivedChar = Serial.read();
        incomingSignal += receivedChar;

        for (size_t i = 0; i < commandCount; ++i) {
            if (incomingSignal == validCommands[i]) {
                Serial.println("Valid command received: " + incomingSignal);
                String validSignal = incomingSignal;
                incomingSignal = "";
                return validSignal;
            }
        }

        bool validPrefix = false;
        for (size_t i = 0; i < commandCount; ++i) {
            if (String(validCommands[i]).startsWith(incomingSignal)) {
                validPrefix = true;
                break;
            }
        }

        if (!validPrefix) {
            Serial.println("Invalid data detected. Clearing buffer: " + incomingSignal);
            incomingSignal = "";
            while (Serial.available() > 0) {
                Serial.read();
            }
        }
    }

    return "";
}

// Sensor Operations
void readSensor(Sensors &sensorData) {
    memcpy(&sensorData.header, HEADER, 8);
    sensorData.loadCell = digitalRead(LOADCELL);
    sensorData.pressure[0] = processPressureValue(analogRead(PRESSSEN1));
    sensorData.pressure[1] = processPressureValue(analogRead(PRESSSEN2));
    sensorData.pressure[2] = processPressureValue(analogRead(PRESSSEN3));
    sensorData.pressure[3] = processPressureValue(analogRead(PRESSSEN4));
    sensorData.pressure[4] = processPressureValue(analogRead(PRESSSEN5));
    sensorData.pressure[5] = processPressureValue(analogRead(PRESSSEN6));
    sensorData.thermocouple[0] = processThermoCoupleValue(analogRead(THERMOCPL1));
    sensorData.thermocouple[1] = processThermoCoupleValue(analogRead(THERMOCPL2));
    sensorData.thermocouple[2] = processThermoCoupleValue(analogRead(THERMOCPL3));
    sensorData.thermocouple[3] = processThermoCoupleValue(analogRead(THERMOCPL4));
    checksum12(&sensorData.checkSum, &sensorData, 52);
}

void checksum12(void *checksum, const void *data, int n) {
    uint8_t* checksumPtr = (uint8_t *) checksum;
    const uint8_t* dataPtr = (uint8_t *) data;
    memset(checksumPtr, 0, 12);
    for (int i = 0; i < n; ++i) {
        checksumPtr[i % 12] ^= dataPtr[i];
        checksumPtr[(i + 1) % 12] ^= (dataPtr[i] >> 4) | (dataPtr[i] << 4);
    }
}

void sendSensor(Sensors &sensorData) {
    readSensor(sensorData);
    memcpy(dataOut, &sensorData, sizeof(Sensors));
    Serial.write(dataOut, sizeof(Sensors));
    Serial.println("Sensor data sent.");
}

// Operational Functions
void nitrogenFlush(int duration) {
    Serial.println("Starting nitrogen flush...");
    openValve(NITROGENVALVE);
    delay(duration);
    closeValve(NITROGENVALVE);
    Serial.println("Nitrogen flush complete.");
}

void ignitionSequence() {
    Serial.println("Starting ignition sequence...");
    nitrogenFlush(10);
    openValve(IGNITERVALVE);
    openValve(FUELVALVE);
    delay(15);
    openValve(OXIDIZERVALVE);
    Serial.println("Ignition sequence complete.");
}

void shutdownSequence() {
    Serial.println("Starting shutdown sequence...");
    closeValve(OXIDIZERVALVE);
    delay(100);
    closeValve(FUELVALVE);
    nitrogenFlush(20);
    Serial.println("Shutdown sequence complete.");
}

// Setup
void setup() {
    pinMode(LOADCELL, INPUT);
    pinMode(PRESSSEN1, INPUT);
    pinMode(PRESSSEN2, INPUT);
    pinMode(PRESSSEN3, INPUT);
    pinMode(PRESSSEN4, INPUT);
    pinMode(PRESSSEN5, INPUT);
    pinMode(PRESSSEN6, INPUT);
    pinMode(THERMOCPL1, INPUT);
    pinMode(THERMOCPL2, INPUT);
    pinMode(THERMOCPL3, INPUT);
    pinMode(THERMOCPL4, INPUT);
    pinMode(NITROGENVALVE, OUTPUT);
    pinMode(OXIDIZERVALVE, OUTPUT);
    pinMode(FUELVALVE, OUTPUT);
    pinMode(IGNITERVALVE, OUTPUT);
    pinMode(PRESFUELVALVE, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);

    Serial.begin(BAUD_RATE);
    Serial.println("Initializing system...");

    const char* startupCommands[] = {CTRLACTV, LOG_STRT};
    for (size_t i = 0; i < 2; ++i) {
        Serial.println("Waiting for command: " + String(startupCommands[i]));
        while (checkData(&startupCommands[i], 1).length() == 0);
        Serial.println("Command received: " + String(startupCommands[i]));
        Serial.write(startupCommands[i]);
    }

    sendSensor(sensorData);
    Serial.println("Setup complete.");
}

// Main Loop
void loop() {
    masterTime = millis();
    const char* validCommands[] = {PRESFUEL, INRTFLSH, PNG, IGN, SHTDWN};

    incomingSignal = checkData(validCommands, sizeof(validCommands) / sizeof(validCommands[0]));

    if (incomingSignal.length() > 0) {
        if (incomingSignal == PNG) {
            timeSinceLastPing = 0;
            lastPingTime = masterTime;
            Serial.println("Ping received. Responding...");
            Serial.write(PNG);
            incomingSignal = "";
            return;
        }

        if (!isBurning) {
            if (incomingSignal == PRESFUEL) {
                Serial.println("Processing PRESFUEL...");
                Serial.write(PRESFUEL);
            } else if (incomingSignal == INRTFLSH) {
                Serial.println("Processing INRTFLSH...");
                nitrogenFlush(10);
                Serial.write(INRTFLSH);
            } else if (incomingSignal == IGN) {
                Serial.println("Processing IGN...");
                ignitionSequence();
                isBurning = true;
                Serial.write(IGN);
            }
        } else {
            if (incomingSignal == SHTDWN) {
                Serial.println("Processing SHTDWN...");
                shutdownSequence();
                Serial.write(SHTDWN);
                isBurning = false;
            }
        }
    }

    timeSinceLastPing = masterTime - lastPingTime;

    if (timeSinceLastPing > 100000) {
        Serial.println("Ping timeout detected. System would shut down!");
    }
}
