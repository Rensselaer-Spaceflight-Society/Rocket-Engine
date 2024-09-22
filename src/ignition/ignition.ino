/*
Open Fuel
Wait 0.2 sec
Open Oxidizer
Wait 0.2 sec
Current to Steel Wool Igniter
*/

constexpr int baud_rate = 9600;

constexpr int fuel_valve_pin = 1; //Fuel Valve Pin (change pin)
constexpr int oxidizer_valve_pin = 2; //Oxidizer Valve Pin (change pin)
constexpr int igniter_pin = 3; //Igniter Pin (change pin)
unsigned long time;

void setup() {
  Serial.begin(baud_rate);
}
void loop() {
  time = millis();
  if (time == 0) {
    digitalWrite(fuel_valve_pin, HIGH); //Open Fuel
    Serial.print("Open Fuel: ");
    Serial.println(time);
  }
  if (time == 200) {
    digitalWrite(oxidizer_valve_pin, HIGH); //Open Oxidizer
    Serial.print("Open Oxidizer: ");
    Serial.println(time);
  }
  if (time == 400) {
    digitalWrite(igniter_pin, HIGH); //Ignition
    Serial.print("Ignition: ");
    Serial.println(time);
  }
}