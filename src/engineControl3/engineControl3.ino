#include <string.h>
#include <time.h>
#include <stdlib.h>

constexpr int BAUD_RATE = 9600;
constexpr int CONTROL_ROOM_RESPONSE_SIZE = 6;

// Command Statements
constexpr char CTRLACTV[] = "CtrlActi";
constexpr char LOG_STRT[] = "LogStart";
constexpr char INRTFLSH[] = "InertFlush";
constexpr char PRESFUEL[] = "PressFuel";
constexpr char IGN[] = "Ignite";
constexpr char SHTDWN[] = "Shutdown";
constexpr char AWK[] = "Acknowledge";
constexpr char PNG[] = "Ping";

// Pin Values (Digital)
constexpr int NITROGENVALVE = 25;
constexpr int OXIDIZERVALVE = 26;
constexpr int FUELVALVE = 27;
constexpr int IGNITERVALVE = 28;
constexpr int PRESFUELVALVE = 15;

// Analog Pins for Pressure Transducers
constexpr int PRESSSEN1 = 0;
constexpr int PRESSSEN2 = 1;
constexpr int PRESSSEN3 = 2;
constexpr int PRESSSEN4 = 3;
constexpr int PRESSSEN5 = 4;
constexpr int PRESSSEN6 = 5;

// Digital Pin for Load Cell
constexpr int LOADCELL = 24;

// Thermocouples Chip Select Pins (Only one can be HIGH at once)
constexpr int THERMOCPL1CS = 22;
constexpr int THERMOCPL2CS = 29;
constexpr int THERMOCPL3CS = 30;
constexpr int THERMOCPL4CS = 31;

// LED Ports (Digital)
constexpr int LED1 = 5;
constexpr int LED2 = 6;
constexpr int LED3 = 7;

// Struct for Sensor Data
struct Sensors {
    float loadCell;
    float thermocouple[4];
    float pressure[6];
};

Sensors sensorData = {0};
char dataIn[16];
char dataOut[sizeof(Sensors)];
String incomingSignal;

int masterTime = 0;
int timeSinceLastPing = 0;
int lastPingTime = 0;

// Utility functions to encapsulate repetitive tasks
void openValve(int pin) {
    digitalWrite(pin, HIGH);
}

void closeValve(int pin) {
    digitalWrite(pin, LOW);
}

void waitTime(int milliseconds) {
    while (millis() < (masterTime + milliseconds)) {
        sendSensor(sensorData); // Continuously send sensor data
    }
}

// Communication functions
void checkComms(const char* expectedSignal, bool sendAck, const char* pingToSend = "NONE") {
    incomingSignal = "";
    while (incomingSignal != expectedSignal) {
        if (Serial.available() >= CONTROL_ROOM_RESPONSE_SIZE) {
            Serial.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
            incomingSignal = String(dataIn);
        }
        if (sendAck && pingToSend != "NONE") {
            Serial.write(pingToSend); // Send custom ping message if needed
        }
    }
    Serial.write(AWK); // Send acknowledgment after matching signal
}

// Sensor-related functions
void readSensor(Sensors& sensorData) {
    sensorData.loadCell = digitalRead(LOADCELL);
    sensorData.pressure[0] = analogRead(PRESSSEN1);
    sensorData.pressure[1] = analogRead(PRESSSEN2);
    sensorData.pressure[2] = analogRead(PRESSSEN3);
    sensorData.pressure[3] = analogRead(PRESSSEN4);
    sensorData.pressure[4] = analogRead(PRESSSEN5);
    sensorData.pressure[5] = analogRead(PRESSSEN6);

    sensorData.thermocouple[0] = digitalRead(THERMOCPL1CS);
    sensorData.thermocouple[1] = digitalRead(THERMOCPL2CS);
    sensorData.thermocouple[2] = digitalRead(THERMOCPL3CS);
    sensorData.thermocouple[3] = digitalRead(THERMOCPL4CS);
}

void sendSensor(Sensors& sensorData) {
    readSensor(sensorData); // Read sensor values before sending
    memcpy(dataOut, &sensorData, sizeof(Sensors));
    Serial.write(dataOut, sizeof(Sensors));
}

// Operations functions
void pressFuel(){

}
void nominalCheck(){
  
}
void nitrogenFlush(int duration) {
    Serial.println("Inert gas flush started");
    openValve(NITROGENVALVE);
    waitTime(duration);
    closeValve(NITROGENVALVE);
    Serial.println("Inert gas flush ended");
}

void ignitionSequence() {
    Serial.println("Ignition Started");
    nitrogenFlush(10);
    openValve(IGNITERVALVE);
    openValve(FUELVALVE);
    waitTime(15);
    openValve(OXIDIZERVALVE);
    Serial.println("Should be burning now");
}

void shutdownSequence() {
    Serial.println("Shutdown started");
    closeValve(OXIDIZERVALVE);
    waitTime(1000);
    closeValve(FUELVALVE);
    waitTime(1000);
    nitrogenFlush(2);
    waitTime(500);
    openValve(OXIDIZERVALVE);
    waitTime(500);
    openValve(NITROGENVALVE);
    Serial.println("Hopefully no more burning");
}

// Setup function
void setup() {
    // Initialize pins
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

    pinMode(NITROGENVALVE, OUTPUT);
    pinMode(OXIDIZERVALVE, OUTPUT);
    pinMode(FUELVALVE, OUTPUT);
    pinMode(IGNITERVALVE, OUTPUT);
    pinMode(PRESFUELVALVE, OUTPUT);
    
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);

    // Start communication
    Serial.begin(BAUD_RATE);
    checkComms(CTRLACTV, false);
    Serial.println("Comms Found");
    digitalWrite(LED1, HIGH);

    checkComms(LOG_STRT, true, CTRLACTV);
    digitalWrite(LED2, HIGH);
    Serial.write(LOG_STRT);

    bool fuelPressed = false;
    bool inertFlushComplete = false;
    masterTime = millis();

    // Wait for ignition command
    while (incomingSignal != IGN) {
        if (millis() - masterTime > 250) {
            masterTime = millis();
            sendSensor(sensorData);
        }
        if (Serial.available() >= CONTROL_ROOM_RESPONSE_SIZE) {
            Serial.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
            incomingSignal = String(dataIn);
        }
        if (incomingSignal == PRESFUEL && !fuelPressed) {
            pressFuel();
            fuelPressed = true;
        }
        if (fuelPressed && !inertFlushComplete) {
            Serial.write(PRESFUEL);
        }
        if (incomingSignal == INRTFLSH && !inertFlushComplete) {
            nitrogenFlush(10);
            inertFlushComplete = true;
        }
        if (fuelPressed && inertFlushComplete) {
            Serial.write(INRTFLSH);
        }
    }
    Serial.write(IGN);
    masterTime = millis();
    ignitionSequence();
}

// Main loop function
void loop() {
    masterTime = millis();
    nominalCheck();
    sendSensor(sensorData);

    if (Serial.available() >= CONTROL_ROOM_RESPONSE_SIZE) {
        Serial.readBytes(dataIn, CONTROL_ROOM_RESPONSE_SIZE);
        incomingSignal = String(dataIn);
    }
    if (incomingSignal == SHTDWN) {
        shutdownSequence();
        Serial.write(SHTDWN);
    } else if (incomingSignal == PNG) {
        timeSinceLastPing = 0;
        lastPingTime = masterTime;
    }

    timeSinceLastPing = masterTime - lastPingTime;
    if (timeSinceLastPing > 250) {
        shutdownSequence();
    }
}
