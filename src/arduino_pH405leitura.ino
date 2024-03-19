const int PH_PIN = A0; // Pino analógico conectado ao sensor de pH

void setup() {
  Serial.begin(9600); // Inicializa a comunicação serial
}

void loop() {
  int sensorValue = analogRead(PH_PIN);

  // Converte a voltagem em pH
  float voltage = sensorValue * (5.0 / 1023.0);
  float calculus = - 1023 * (voltage / 5.0) + 1023 ;
  float pH = calculus * 14 / 1023;
  float pHcalibrado = 2.0373*pH - 6.705; 

  // Imprime o valor de pH no Serial Monitor
  Serial.print("pH: ");
  Serial.print(pHcalibrado);
  Serial.print(" Voltagem: ");
  Serial.println(voltage);

  delay(1000); // Aguarda 1 segundo antes de ler novamente o valor de pH
}

