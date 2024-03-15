#include <OneWire.h>
#include <DallasTemperature.h>

// Pino onde o sensor está conectado
#define ONE_WIRE_BUS 2 // Altere para o pino correto do seu Arduino

// Inicializa o objeto OneWire
OneWire oneWire(ONE_WIRE_BUS);

// Inicializa o objeto DallasTemperature para manipular o sensor
DallasTemperature sensors(&oneWire);

void setup() {
  // Inicializa a comunicação serial para monitoramento
  Serial.begin(9600);
  
  // Inicializa o sensor
  sensors.begin();
}

void loop() {
  // Chama a função para obter as temperaturas dos dispositivos conectados
  sensors.requestTemperatures(); 

  // Obtém a temperatura em Celsius
  float temperaturaCelsius = sensors.getTempCByIndex(0);

  if (temperaturaCelsius != DEVICE_DISCONNECTED_C) {
    Serial.print("Temperatura: ");
    Serial.print(temperaturaCelsius);
    Serial.println(" °C");
  } else {
    Serial.println("Erro ao ler o sensor de temperatura!");
  }
  delay(1000);
}

