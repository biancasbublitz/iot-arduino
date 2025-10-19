#define sensorPin 21; //Pino D0 do sensor conectado na porta 21 do Arduino
int readingSensor;
int count = 0;

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
}

void loop() {
  readingSensor = digitalRead(sensorPin);

  if (readingSensor == 1) {
    count++; 

    Serial.print("Numero de deteccoes: ");
    Serial.println(count);

    delay(500);
  }
}