#include <Ultrasonic.h>

#define sensorTrigger 12;
#define sensorEcho 13;

//Inicializa o sensor
Ultrasonic ultrasonic(sensorTrigger, sensorEcho);

void setup() {
  Serial.begin(9600);
}

void loop() {
  //Le as informacoes do sensor em cm e pol
  float cmMsec, inMsec;
  long microsec = ultrasonic.timing();

  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  inMsec = ultrasonic.convert(microsec, Ultrasonic::IN);

  Serial.print("Distancia em cm: ");
  Serial.print(cmMsec);

  Serial.print("Distancia em pol: ");
  Serial.println(inMsec);

  delay(1000);
}
