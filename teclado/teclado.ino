#include <Keypad.h>

const byte LINHAS = 4;
const byte COLUNAS = 4;

const char TECLAS_MATRIZ[LINHAS][COLUNAS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte PINOS_LINHAS[LINHAS] = {32, 33, 25, 26}; // Pinos de conexao com as linhas do teclado
byte PINOS_COLUNAS[COLUNAS] = {27, 17, 12, 13}; // Pinos de conexao com as colunas do teclado

Keypad teclado_personalizado = Keypad(makeKeymap(TECLAS_MATRIZ), PINOS_LINHAS, PINOS_COLUNAS, LINHAS, COLUNAS);

void setup() {
  Serial.begin(9600);
}

void loop() {

  char leitura_teclas = teclado_personalizado.getKey();

  if (leitura_teclas) {
    Serial.println(leitura_teclas);
  }
  
}