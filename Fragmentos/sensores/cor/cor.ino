#include <Wire.h>
#include <SparkFun_APDS9960.h>

#define SDA_PIN 21
#define SCL_PIN 22

SparkFun_APDS9960 apds = SparkFun_APDS9960();

uint16_t ambient_light = 0;
uint16_t red_light = 0;
uint16_t green_light = 0;
uint16_t blue_light = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println();
  Serial.println(F("--------------------------------"));
  Serial.println(F("APDS-9960 - Sensor de Cor (ESP32)"));
  Serial.println(F("--------------------------------"));
  
  if (apds.init()) {
    Serial.println(F("APDS-9960: Inicializacao completa!"));
  } else {
    Serial.println(F("APDS-9960: Erro na inicializacao!"));
  }
  
  if (apds.enableLightSensor(false)) {
    Serial.println(F("Sensor de luz funcionando!"));
  } else {
    Serial.println(F("Erro ao habilitar sensor de luz!"));
  }

  delay(500); // tempo de calibracao
}

void loop() {
  if (!apds.readAmbientLight(ambient_light) ||
      !apds.readRedLight(red_light) ||
      !apds.readGreenLight(green_light) ||
      !apds.readBlueLight(blue_light)) {
    Serial.println("Erro ao ler os valores de luminosidade");
  } else {
    Serial.print("Ambiente: ");
    Serial.print(ambient_light);
    Serial.print(" | Vermelho: ");
    Serial.print(red_light);
    Serial.print(" | Verde: ");
    Serial.print(green_light);
    Serial.print(" | Azul: ");
    Serial.println(blue_light);
  }

  delay(1000);
}
