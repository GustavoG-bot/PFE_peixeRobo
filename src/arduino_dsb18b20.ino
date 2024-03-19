#include <OneWire.h>
#include <DallasTemperature.h>

// Pino onde o sensor está conectado
#define ONE_WIRE_BUS 2 // Altere para o pino correto do seu Arduino
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  sensors.begin();
}

void loop() {
  sensors.requestTemperatures(); 

  float temperaturaCelsius = sensors.getTempCByIndex(0);
  float tempcorr = 0.9964 * temperaturaCelsius - 0.2338;

  if (temperaturaCelsius != DEVICE_DISCONNECTED_C) {
    Serial.print("Temperatura: ");
    Serial.print(tempcorr);
    Serial.println(" °C");
  } else {
    Serial.println("Erro ao ler o sensor de temperatura!");
  }
  delay(1000);
}

