// Define the pin where the sensor is connected
const int sensorPin = A0;

void setup() {
  // Initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

void loop() {
  // Read the input on analog pin 0:
  int sensorValue = analogRead(sensorPin);
  
  // Convert the analog reading (which goes from 0 - 1024) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1024.0);
  
  // Determine turbidity level based on voltage
  String turbidityLevel = determineTurbidityLevel(voltage);
  
  // Print out the voltage and turbidity level
  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.print(" V, Turbidity Level: ");
  Serial.println(turbidityLevel);
  
  // Delay for a bit to avoid spamming
  delay(1000);
}

String determineTurbidityLevel(float voltage) {
 if (voltage < 0.6) {
    return "Muito Turvo";
 } else if (voltage < 1.5) {
    return "Turvo";
 } else if (voltage < 2.5) {
    return "Moderadamente Turvo";
 } else if (voltage < 3.5) {
    return "Ligeiramente Turvo";
 }  else {
    return "Claro";
 }
}