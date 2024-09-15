void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int sensorValue = analogRead(A3);
  float voltage = sensorValue * (5 / 1023);
  Serial.print("Analog = ");
  Serial.print(sensorValue);
  Serial.println();
  Serial.print("Voltage = ");
  Serial.print(voltage);
  Serial.println();
}
