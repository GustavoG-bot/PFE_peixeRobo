// Inclusão das bibliotecas necessárias.
#include <Arduino.h>               // Biblioteca principal do Arduino.
#include <arduino-timer.h>         // Biblioteca para manipulação de timers.
#include <Q2HX711.h>               // Biblioteca para o sensor HX711, usado para leitura de células de carga.
#include <Servo.h>                 // Biblioteca para controlar servomotores.
#include <OneWire.h>               // Biblioteca para comunicação com dispositivos OneWire.
#include <DallasTemperature.h>     // Biblioteca para trabalhar com o sensor de temperatura Dallas.

// Declaração dos objetos e variáveis globais.
Servo myServo;                      // Objeto para controlar o servo.
auto timer = timer_create_default();// Cria um timer padrão para agendar tarefas periódicas.
Q2HX711 MPS20N0040D(4, 13);         // Objeto do sensor de pressão (possivelmente usado para medir a profundidade).
OneWire oneWire(5);                 // Objeto OneWire para comunicação com dispositivos OneWire, como sensores de temperatura.
DallasTemperature sensors(&oneWire);// Objeto para gerenciar o sensor de temperatura Dallas.

const int pH_Pin = A0;              // Pino analógico ligado ao módulo de pH.
const int turb_Pin = A1;            // Pino analógico ligado ao módulo de turbidez.

// Configuração dos pinos para os sensores de limite e motor.
const int limitSwitchMin_Pin = 3;   // Pino para o interruptor de limite mínimo.
const int limitSwitchMax_Pin = 2;   // Pino para o interruptor de limite máximo.
const int motor_Pin = 10;           // Pino PWM para o motor (servo).

// Variáveis para armazenar os valores dos sensores.
float temp_Value;                   // Armazena a temperatura.
float pH_Value;                     // Armazena o valor do pH.
int turb_Value;                     // Armazena a turbidez.

// Variáveis de controle.
bool mode;                          // Modo de operação, pode ser auto ou manual.
float depth_Real;                   // Profundidade real medida.
float depth_User;                   // Profundidade desejada pelo usuário.
String serialData = "";             // Armazena os dados recebidos via serial.
bool limitSwitchMin;                // Estado do interruptor de limite mínimo.
bool limitSwitchMax;                // Estado do interruptor de limite máximo.
bool enable;                        // Habilita ou desabilita o motor.
bool dir;                           // Direção do motor, 1 para subir.

// Variáveis para debouncing dos interruptores de limite.
unsigned long debounceDelay = 200;               
volatile unsigned long lastDebounceTimeMin = 0;  
volatile unsigned long lastDebounceTimeMax = 0;

bool readSerialData() {
  // Verifica se há dados disponíveis para leitura na porta serial
  while (Serial.available() > 0) {
    // Lê o próximo caractere disponível
    char ch = Serial.read();
    // Acrescenta o caractere lido à variável serialData
    serialData += ch;

    // Verifica se o caractere é um final de linha (por exemplo, nova linha ou retorno de carro)
    if (ch == '\n' || ch == '\r') {
      // Remove espaços em branco à frente e atrás (incluindo finais de linha)
      serialData.trim();

      // Compara serialData com os comandos esperados e executa ações correspondentes
      if (serialData == "auto") {
        mode = 1; // Modo automático
      } else if (serialData == "manual") {
        mode = 0; // Modo manual
      } else if (serialData == "up" && mode == 0) {
        enable = true;
        dir = false;
      } else if (serialData == "down" && mode == 0) {
        enable = true;
        dir = true;
      } else if (serialData == "stop") {
        enable = false;
      } else {
        // É o caso em que o usuário digita a referência de profundidade
        depth_User = atof(serialData.c_str()); // Converte string para float
      }
      // Reseta a variável serialData
      serialData = "";
    }
  }
  return true;
}

bool updateSensors(void*) {
  // Medição de temperatura
  sensors.requestTemperatures();
  temp_Value = 0.9964 * sensors.getTempCByIndex(0) - 0.2338; // Resultado da calibração
  // Medição de pH
  float pH_Voltage = analogRead(pH_Pin) * (5.0/1023.0);
  float calculus = - 1024.0 * pH_Voltage / 5.0 + 1024.0;
  float ph_Value = calculus * 14.0 / 1024.0;
  pH_Value = 2.0373 * ph_Value  - 6.705; // Resultado da calibração
  // Medição de turbidez
  float turbidityVoltage = analogRead(turb_Pin) * (5.0 / 1023.0);
  // Resultado da calibração qualitativa
  if (turbidityVoltage < 0.6) {
    turb_Value = 5;
  } else if (turbidityVoltage < 1.5) {
    turb_Value = 4;
  } else if (turbidityVoltage < 2.5) {
    turb_Value = 3;
  } else if (turbidityVoltage < 3.5) {
    turb_Value = 2;
  } else {
    turb_Value = 1;
  }
  // Imprime valores na Serial
  Serial.print("Temperatura (°C): ");
  Serial.println(temp_Value);
  Serial.print("pH: ");
  Serial.println(pH_Value);
  Serial.print("Turbidez (1 a 5): ");
  Serial.println(turb_Value);
  Serial.println(" ");
  Serial.println("Profundidade (cm): ");
  Serial.println(depth_Real);
  Serial.println(" ");
  return true;
}

bool motorMain(void*) {
  // Verifica se o motor está habilitado
  if (enable) {
    if (dir) {
      myServo.write(0);   // Esvaziar
    } else {
      myServo.write(135); // Encher
    }
  } else {
    // Para o motor quando desabilitado
    myServo.write(92);
  }
  return true;
}

bool compareDepth(void*) {
  // Medição de profundidade a partir do sensor de pressão diferencial  
  float v = 5.0 * MPS20N0040D.read() / (128 * (pow(2, 24) - 1));
  depth_Real = 7.0712 * v * 1000 - 151.56 + 4.0;

  // Variáveis para verificar se algum fim de curso está ativo
  bool limitSwitchMaxActive = digitalRead(limitSwitchMax_Pin) == LOW ? true : false;
  bool limitSwitchMinActive = digitalRead(limitSwitchMin_Pin) == LOW ? true : false;

  // Caso em modo autônomo, alterar o sentido de rotação do motor em função da distância do robô à referência
  if (mode == 1) {
    if ((depth_Real <= depth_User - 2) && (limitSwitchMaxActive == false)) {
      enable = true;
      dir = true;
    } else if ((depth_Real > depth_User + 2) && (limitSwitchMinActive == false)) {
      enable = true;
      dir = false;
    } else {
      enable = false;
    }
  }
  return true;
}

bool disableMotor(void* arg) {
  // Desabilita o motor após o tempo determinado
  enable = false;
  return true;
}

bool limitSwitchMinISR(void*) {
  // Acionar o motor no sentido contrário por um certo tempo para desativar fim de curso
  unsigned long currentTime = millis(); // Obtém o tempo atual em milissegundos desde que o programa começou

  // Verifica se passou tempo suficiente desde a última interrupção para evitar ruído (debouncing)
  if (currentTime - lastDebounceTimeMin > debounceDelay) {
    // Atualiza o último tempo de debounce
    lastDebounceTimeMin = currentTime;

    // Verifica se o fim de curso ainda está acionado
    if (digitalRead(limitSwitchMin_Pin) == LOW) {
      enable = true;
      dir = false;   // Inverte sentido de rotação
      timer.in(300, disableMotor); // Agenda a função 'disableMotor' para ser chamada após 300 ms
    }
  }
  return true;
}

bool limitSwitchMaxISR(void*) {
  // Acionar o motor no sentido contrário por um certo tempo para desativar fim de curso
  unsigned long currentTime = millis(); // Obtém o tempo atual em milissegundos desde que o programa começou

  // Verifica se passou tempo suficiente desde a última interrupção para evitar ruído (debouncing)
  if (currentTime - lastDebounceTimeMax > debounceDelay) {
    // Atualiza o último tempo de debounce
    lastDebounceTimeMax = currentTime;

    // Verifica se o fim de curso ainda está acionado
    if (digitalRead(limitSwitchMax_Pin) == LOW) {
      enable = true; 
      dir = true;    // Inverte sentido de rotação
      timer.in(300, disableMotor); // Agenda a função 'disableMotor' para ser chamada após 300 ms
    }
  }
  return true;
}

// Função setup() é chamada uma vez quando o sketch é iniciado.
void setup() {
  Serial.begin(9600);               // Inicia a comunicação serial com uma taxa de 9600 bps.
  myServo.attach(motor_Pin);        // Anexa o servomotor ao pino correspondente.
  // Configurações de pinos e interrupções para os sensores de fim de curso.
  pinMode(limitSwitchMin_Pin, INPUT_PULLUP);
  pinMode(limitSwitchMax_Pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(limitSwitchMin_Pin), limitSwitchMinISR, FALLING); // borda de descida
  attachInterrupt(digitalPinToInterrupt(limitSwitchMax_Pin), limitSwitchMaxISR, FALLING); // borda de descida

  // Define os timers para leitura de dados seriais, comparação de profundidade, controle do motor 
  // e atualização dos sensores.
  timer.every(500, readSerialData);
  timer.every(1000, compareDepth);
  timer.every(50, motorMain);
  timer.every(2000, updateSensors);
}

// A função loop() é chamada infinitamente
void loop() {
  timer.tick();  // Atualiza e verifica os timers, 
                 // executando as funções associadas quando necessário.
}