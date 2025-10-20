#include <DHT.h>

#define DHTPIN 17 //Pino do sensor conectado na porta 17 do Arduino
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  delay(2000); //Intervalo de dois segundos entre as medições

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) { //Verifica se a umidade ou temperatura são ou não um número
    return;
  }

  Serial.print("Umidade: ");
  Serial.print(h);
  Serial.println("%");

  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println("°C ");
}