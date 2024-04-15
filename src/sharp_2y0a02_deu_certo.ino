const int sensorPin = A0;

// Convert voltage to distance (Placeholder function - replace with actual formula)
float voltageToDistance(float voltage) {
  float distance = 198.17 * exp(-0.82*voltage); // Example formula
  return distance;
}

void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 bps
}

void loop() {
  int sensorValue = analogRead(sensorPin); // Read the input on analog pin 0
  float voltage = sensorValue * (5.0 / 1023.0); // Convert to voltage
  
  // Now you can call voltageToDistance here if you need to use it
  float distance = voltageToDistance(voltage); // Convert voltage to distance
  

  Serial.print(", Voltage: ");
  Serial.print(voltage);
  Serial.print(", Distance: ");
  Serial.println(distance); // Print distance if needed
  
  delay(500); // Delay for a bit to avoid spamming
}