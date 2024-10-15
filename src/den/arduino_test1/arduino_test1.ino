void setup() {
  Serial1.begin(9600);
}

void loop() {
  while (Serial1.available() == 0) {}     
  String input_str = Serial1.readString();  
  
  if (input_str == "Hello from Qt") {      // Check to see 
    Serial1.println("Received correct data");
    Serial1.println(input_str);        // Send data back to control center
  } else {
    Serial1.println("Incorrect data received");
  }
}
