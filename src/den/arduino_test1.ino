void setup() {
  Serial.begin(9600);
}

void loop() {
  while (Serial.available() == 0) {}     
  String input_str = Serial.readString();  
  
  if (teststr == "Hello from Qt") {      // Check to see 
    Serial.println("Received correct data");
    Serial.write(teststr);        // Send data back to control center
  } else {
    Serial.println("Incorrect data received");
  }
}
