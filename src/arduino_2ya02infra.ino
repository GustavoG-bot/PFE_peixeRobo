#define IR_PIN A0 // Pino analógico onde o sensor está conectado

void setup() {
  // Inicializa a comunicação serial para monitoramento
  Serial.begin(9600);
}

void loop() {
  // Lê o valor analógico do sensor
  int sensorValue = analogRead(IR_PIN);

  // Converte o valor lido para volts
  float volts = sensorValue * 0.0048828125;  // value from sensor * (5/1024)

  // Calcula a distância usando uma fórmula adequada
  // A fórmula real para este sensor não é uma relação linear, portanto, um valor tabelado ou calibrado seria mais apropriado
  float distance = 13.0 * pow(volts, -1.0); // fórmula ajustada, a potência deve ser -1.0

  // Imprime a distância na porta serial
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Aguarda um segundo antes de fazer a próxima leitura
  delay(1000);
}
