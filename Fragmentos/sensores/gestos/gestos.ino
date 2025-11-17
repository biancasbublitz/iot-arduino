#include <Wire.h>
#include <SparkFun_APDS9960.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define APDS9960_INT 33

SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;

void IRAM_ATTR interruptRoutine() {
  isr_flag = 1;
}

void setup() {
  Serial.begin(9600);
  Wire.begin(SDA_PIN, SCL_PIN);
  
  pinMode(APDS9960_INT, INPUT);
  
  Serial.println(F("--------------------------------"));
  Serial.println(F("APDS-9960 - Teste de Gestos (ESP32)"));
  Serial.println(F("--------------------------------"));
  
  attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);

  if (apds.init()) {
    Serial.println(F("APDS-9960: Inicializacao completa!"));
  } else {
    Serial.println(F("Erro ao inicializar APDS-9960!"));
  }
  
  if (apds.enableGestureSensor(true)) {
    Serial.println(F("Sensor de gestos funcionando!"));
  } else {
    Serial.println(F("Erro ao iniciar sensor de gestos!"));
  }
}

void loop() {
  if (isr_flag == 1) {
    detachInterrupt(digitalPinToInterrupt(APDS9960_INT));
    handleGesture();
    isr_flag = 0;
    attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);
  }
}

void handleGesture() {
  if (apds.isGestureAvailable()) {
    switch (apds.readGesture()) {
      case DIR_UP: Serial.println("CIMA"); break;
      case DIR_DOWN: Serial.println("BAIXO"); break;
      case DIR_LEFT: Serial.println("ESQUERDA"); break;
      case DIR_RIGHT: Serial.println("DIREITA"); break;
      case DIR_NEAR: Serial.println("APROXIMANDO"); break;
      case DIR_FAR: Serial.println("DISTANCIANDO"); break;
      default: Serial.println("GESTO NAO RECONHECIDO");
    }
  }
}
