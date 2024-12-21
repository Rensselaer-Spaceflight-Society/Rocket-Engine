#ifndef __PINS_H_
#define __PINS_H_

// Pin Values (Digital)
constexpr int NITROGENVALVE = 25;
constexpr int PRESFUELVALVE = 26;
constexpr int FUELVALVE = 27;
constexpr int OXIDIZERVALVE = 28;
constexpr int IGNITERVALVE = 29;

// Analog Pins for Pressure Transducers
constexpr int PRESSSEN1 = 0;
constexpr int PRESSSEN2 = 1;
constexpr int PRESSSEN3 = 2;
constexpr int PRESSSEN4 = 3;
constexpr int PRESSSEN5 = 4;
constexpr int PRESSSEN6 = 5;

// Digital Pin for Load Cell
constexpr int LOADCELL = 24;

// Thermocouple Analog Pins
constexpr int THERMOCPL1 = 6;
constexpr int THERMOCPL2 = 7;
constexpr int THERMOCPL3 = 8;
constexpr int THERMOCPL4 = 9;

// LED Ports (Digital)
constexpr int LED1 = 30;
constexpr int LED2 = 31;
constexpr int LED3 = 32;

void setupPins() {
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
}

void resetPins() {
  digitalWrite(NITROGENVALVE, LOW);
  digitalWrite(OXIDIZERVALVE, LOW);
  digitalWrite(FUELVALVE, LOW);
  digitalWrite(IGNITERVALVE, LOW);
  digitalWrite(PRESFUELVALVE, LOW);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
}

#endif  // __PINS_H_