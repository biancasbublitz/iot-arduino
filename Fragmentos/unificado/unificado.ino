#include <Arduino.h>

// ======== BIBLIOTECAS ========
#include <Wire.h>
#include <SparkFun_APDS9960.h>   // Sensor de cor e gestos
#include <Ultrasonic.h>          // Sensor de distância (atualizado)
#include <Keypad.h>              // Teclado matricial
#include <DHT.h>                 // Sensor de umidade/temperatura DHT
#include <OneWire.h>             // Sensor DS18B20
#include <DallasTemperature.h>   // Biblioteca Dallas para DS18B20

// ======== DEFINIÇÕES GERAIS ========
#define SENSOR_MODE "distancia"
// troque entre: "cor", "distancia", "gestos", "joystick", "teclado", "umidade", "velocidade", "rele", "temperatura", "vibracao"

// ======== SENSOR: COR e GESTOS (APDS9960) ========
#define SDA_PIN 21
#define SCL_PIN 22
#define APDS9960_INT 33
SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint16_t ambient_light = 0, red_light = 0, green_light = 0, blue_light = 0;
int isr_flag = 0;
void IRAM_ATTR interruptRoutine() { isr_flag = 1; }

// ======== SENSOR: DISTÂNCIA (Ultrasonic) ========
#define sensorTrigger 12
#define sensorEcho 13
Ultrasonic ultrasonic(sensorTrigger, sensorEcho);

// ======== SENSOR: JOYSTICK ========
#define sensorPinX 25
#define sensorPinY 26
#define sensorPinButton 27
#define X_CORRECTION 128
#define Y_CORRECTION 128
struct button { byte pressed = 0; };
struct analogJoy { short x, y; button button; };

// ======== SENSOR: TECLADO ========
const byte LINHAS = 4, COLUNAS = 4;
const char TECLAS_MATRIZ[LINHAS][COLUNAS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte PINOS_LINHAS[LINHAS] = {32, 33, 25, 26};
byte PINOS_COLUNAS[COLUNAS] = {27, 17, 12, 13};
Keypad teclado_personalizado = Keypad(makeKeymap(TECLAS_MATRIZ), PINOS_LINHAS, PINOS_COLUNAS, LINHAS, COLUNAS);

// ======== SENSOR: UMIDADE/TEMP (DHT22) ========
#define DHTPIN 17
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ======== SENSOR: VELOCIDADE ========
#define sensorSpeedPin 21
int readingSensor, countDeteccao = 0;

// ======== ATUADOR: RELÉ ========
#define pinRele 17  // pino de controle do relé

// ======== ATUADOR: VIBRAÇÃO ========
#define pinVibracao 25  // pino de controle do relé

// ======== SENSOR: TEMPERATURA DS18B20 ========
#define ONE_WIRE_BUS 22
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ======== SETUP ========
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Sistema Unificado de Sensores IoT ===");

  if (strcmp(SENSOR_MODE, "cor") == 0) {
    Wire.begin(SDA_PIN, SCL_PIN);
    if (apds.init()) apds.enableLightSensor(false);
  }
  else if (strcmp(SENSOR_MODE, "gestos") == 0) {
    Wire.begin(SDA_PIN, SCL_PIN);
    pinMode(APDS9960_INT, INPUT);
    attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);
    if (apds.init()) apds.enableGestureSensor(true);
  }
  else if (strcmp(SENSOR_MODE, "umidade") == 0) {
    dht.begin();
  }
  else if (strcmp(SENSOR_MODE, "joystick") == 0) {
    pinMode(sensorPinButton, INPUT_PULLUP);
  }
  else if (strcmp(SENSOR_MODE, "velocidade") == 0) {
    pinMode(sensorSpeedPin, INPUT);
  }
  else if (strcmp(SENSOR_MODE, "rele") == 0) {
    pinMode(pinRele, OUTPUT);
    digitalWrite(pinRele, HIGH); // desligado inicialmente
  }
  else if (strcmp(SENSOR_MODE, "vibracao") == 0) {
    pinMode(pinVibracao, OUTPUT);
    digitalWrite(pinVibracao, HIGH); // desligado inicialmente
  }
  else if (strcmp(SENSOR_MODE, "temperatura") == 0) {
    sensors.begin();
  }

  Serial.print("Modo selecionado: ");
  Serial.println(SENSOR_MODE);
}

// ======== LOOP ========
void loop() {
  if (strcmp(SENSOR_MODE, "cor") == 0) lerCor();
  else if (strcmp(SENSOR_MODE, "gestos") == 0) lerGestos();
  else if (strcmp(SENSOR_MODE, "distancia") == 0) lerDistancia();
  else if (strcmp(SENSOR_MODE, "joystick") == 0) lerJoystick();
  else if (strcmp(SENSOR_MODE, "teclado") == 0) lerTeclado();
  else if (strcmp(SENSOR_MODE, "umidade") == 0) lerDHT();
  else if (strcmp(SENSOR_MODE, "velocidade") == 0) lerVelocidade();
  else if (strcmp(SENSOR_MODE, "rele") == 0) controlarRele();
  else if (strcmp(SENSOR_MODE, "vibracao") == 0) controlarVibracao();
  else if (strcmp(SENSOR_MODE, "temperatura") == 0) lerTemperatura();
  else Serial.println("Sensor não reconhecido.");

  //delay(1000);
}

// ======== FUNÇÕES DE LEITURA ========

void lerCor() {
  if (!apds.readAmbientLight(ambient_light) ||
      !apds.readRedLight(red_light) ||
      !apds.readGreenLight(green_light) ||
      !apds.readBlueLight(blue_light)) {
    Serial.println("Erro ao ler cor.");
  } else {
    Serial.printf("Ambiente: %d | R: %d | G: %d | B: %d\n", ambient_light, red_light, green_light, blue_light);
  }
}

void lerGestos() {
  if (isr_flag == 1) {
    detachInterrupt(digitalPinToInterrupt(APDS9960_INT));
    if (apds.isGestureAvailable()) {
      switch (apds.readGesture()) {
        case DIR_UP: Serial.println("CIMA"); break;
        case DIR_DOWN: Serial.println("BAIXO"); break;
        case DIR_LEFT: Serial.println("ESQUERDA"); break;
        case DIR_RIGHT: Serial.println("DIREITA"); break;
        case DIR_NEAR: Serial.println("APROXIMANDO"); break;
        case DIR_FAR: Serial.println("DISTANCIANDO"); break;
        default: Serial.println("GESTO NÃO RECONHECIDO");
      }
    }
    isr_flag = 0;
    attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);
  }
}

void lerDistancia() {
  long microsec = ultrasonic.timing();
  float cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  float inMsec = ultrasonic.convert(microsec, Ultrasonic::IN);

  Serial.printf("📏 Distância: %.2f cm | %.2f pol\n", cmMsec, inMsec);
}

void lerJoystick() {
  analogJoy joy;
  joy.x = map(analogRead(sensorPinX), 0, 4095, -128, 127);
  joy.y = map(analogRead(sensorPinY), 0, 4095, -128, 127);
  joy.button.pressed = (digitalRead(sensorPinButton) == LOW);
  Serial.printf("🎮 X:%d | Y:%d | Botão: %s\n", joy.x, joy.y, joy.button.pressed ? "Pressionado" : "Solto");
}

void lerTeclado() {
  char tecla = teclado_personalizado.getKey();
  if (tecla) Serial.printf("⌨️ Tecla: %c\n", tecla);
}

void lerDHT() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Erro ao ler DHT!");
    return;
  }
  Serial.printf("💧 Umidade: %.1f %% | 🌡️ Temperatura: %.1f °C\n", h, t);
}

void lerVelocidade() {
  readingSensor = digitalRead(sensorSpeedPin);
  if (readingSensor == HIGH) {
    countDeteccao++;
    Serial.printf("⚙️ Detecções: %d\n", countDeteccao);
    delay(500);
  }
}

// ======== NOVOS COMPONENTES ========

// 🔌 RELÉ
void controlarRele() {
  digitalWrite(pinRele, LOW);  // Liga
  Serial.println("💡 Relé ligado");
  delay(9000);
  digitalWrite(pinRele, HIGH); // Desliga
  Serial.println("💡 Relé desligado");
  delay(9000);
}

// 💭 VIBRAÇÃO
void controlarVibracao() {
  digitalWrite(pinVibracao, LOW);  // Liga
  Serial.println("💡 Vibração desligada");
  delay(9000);
  digitalWrite(pinVibracao, HIGH); // Desliga
  Serial.println("💡 Vibração ligada");
  delay(9000);
}

// 🌡️ TEMPERATURA (DS18B20)
void lerTemperatura() {
  Serial.print("Solicitando temperatura...");
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);

  if (tempC != DEVICE_DISCONNECTED_C) {
    Serial.printf("🌡️ Temperatura DS18B20: %.2f °C\n", tempC);
  } else {
    Serial.println("Erro: Sensor DS18B20 não detectado!");
  }
}
