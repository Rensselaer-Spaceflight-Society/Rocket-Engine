// Pin where AD8495 is connected
const int thermocouplePin = A0;

// Analog reference voltage (5V for most Arduinos, 3.3V for others)
const float analogReferenceVoltage = 5;

// AD8495's scale factor in mV/°C
const float scaleFactor = 5.0; // 5 mV/°C

void setup() {
  Serial.begin(38400);
}

void loop() {
  int analogValue = analogRead(thermocouplePin);
  float voltageC = (analogValue * (analogReferenceVoltage / 1023.0)) /0.005;
  float voltageF = (voltageC * 9/5) + 32;

  Serial.print("Raw analog value: ");
  Serial.println(analogValue);
  Serial.print("Converted voltage celsius: ");
  Serial.println(voltageC);
  Serial.print("Converted voltage fahrenheit: ");
  Serial.println(voltageF);
  Serial.println("...");

  delay(500); // Delay for 1 second before next reading
}