int pinRele = 17; //Definição do pino de controle do relé

void setup() {
  Serial.begin(9600);
  pinMode(pinRele, OUTPUT);
}

void loop() {

  digitalWrite(pinRele, LOW);
  Serial.println("ligada");
  delay(9000); //Intervalo de 9 segundos

  digitalWrite(pinRele, HIGH);
  Serial.println("desligada");
  delay(9000);
}
