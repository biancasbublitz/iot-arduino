#include <Arduino.h>

// ======== BIBLIOTECAS ========
#include <Wire.h>
#include <SparkFun_APDS9960.h>   // Sensor de cor e gestos
#include <Keypad.h>              // Teclado matricial
#include <DHT.h>                 // Sensor umidade/temp
#include <OneWire.h>             // DS18B20
#include <DallasTemperature.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define LED_VERDE  15
#define LED_AMARELO 13
#define LED_VERMELHO 12

bool portaEstaAberta = false;
unsigned long portaAbertaDesde = 0;



// ======== CONFIG Wi-Fi + API ========
const char* WIFI_SSID = "iot2022";
const char* WIFI_PASS = "S3nhab0@";

// 🔥 API PRINCIPAL
String API_URL = "https://api-cogr.onrender.com/readings";

// 🔥 API DE VALIDAÇÃO
String API_VALIDATE = "https://api-cogr.onrender.com/auth/validate";
String API_VALIDATE_TEMP = "https://api-cogr.onrender.com/auth/validate-temperature";


// ======== COMPONENT IDs DO RENDER ========
String ID_TEMP_DS18B20     = "99efc3b2-3ea5-4e41-8343-d237238cf5f1";
String ID_APDS9960         = "9a790730-52f5-4dcd-8504-cd9236cd27a2";
String ID_VELOCIDADE       = "fd5b3cbd-dc53-482f-b9cd-569600cd2db2";
String ID_DISTANCIA        = "63837b13-1cc1-4ba1-93ab-62d64b5719c8";
String ID_RELE             = "1335bab9-5f60-4619-a73c-8070deded4c3";
String ID_SERVO            = "cac9f30e-243a-4feb-87ae-67c98f7c16da";
String ID_JOYSTICK         = "10289229-43b3-4db1-9580-63b5b76e4465";
String ID_TECLADO          = "9dc544ee-36bd-4a52-a69a-f4cf8cffe578";

// ======== SELETOR DE MODO ========
#define SENSOR_MODE "esp2"

// ======== FUNÇÃO HTTP PARA RENDER ========
void enviarParaAPI(String json) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi OFF — reconectando...");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    return;
  }

  HTTPClient http;
  http.begin(API_URL);
  http.addHeader("Content-Type", "application/json");

  Serial.print("➡️ Enviando JSON: ");
  Serial.println(json);

  int codigo = http.POST(json);
  Serial.print("📡 HTTP: ");
  Serial.println(codigo);

  http.end();
}

void apagarTodosLeds() {
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO, LOW);
}


// ======== APDS9960 ========
#define SDA_PIN 21
#define SCL_PIN 22
#define APDS9960_INT 33
SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint16_t ambient_light, red_light, green_light, blue_light;
int isr_flag = 0;
void IRAM_ATTR interruptRoutine() { isr_flag = 1; }

// ======== JOYSTICK ========
#define sensorPinX 25
#define sensorPinY 26
#define sensorPinButton 27
struct button { byte pressed = 0; };
struct analogJoy { short x, y; button button; };

// ======== TECLADO ========
const byte LINHAS = 4, COLUNAS = 4;
const char TECLAS[LINHAS][COLUNAS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte PIN_LINHAS[LINHAS] = {36, 37, 38, 39};
byte PIN_COLUNAS[COLUNAS] = {12, 13, 15, 2};
Keypad teclado = Keypad(makeKeymap(TECLAS), PIN_LINHAS, PIN_COLUNAS, LINHAS, COLUNAS);

// ======== DHT22 ========
#define DHTPIN 12
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ======== VELOCIDADE ========
#define sensorSpeedPin 13
int countDeteccao = 0;

// ======== RELÉ ========
#define pinRele 12

// ======== VIBRAÇÃO ========
#define pinVibracao 21

// ======== TEMPERATURA DS18B20 ========
#define ONE_WIRE_BUS 22
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ======================================================
// ========== VARIÁVEIS DO ESP1 (Senha) =================
// ======================================================
String senhaDigitada = "";
bool capturando = false;

// ======================================================
// =========== FUNÇÃO VALIDAR SENHA VIA API ============
// ======================================================
int validarSenha(String senha) {

  // 🔥🔥🔥 NOVO — remove o asterisco antes de enviar para a API
  if (senha.startsWith("*")) {
    senha = senha.substring(1);
  }
  // 🔥🔥🔥 FIM DA ALTERAÇÃO

  HTTPClient http;
  http.begin(API_VALIDATE);
  http.addHeader("Content-Type", "application/json");

  String body = "{\"password\":\"" + senha + "\"}";
  Serial.println("➡ Validando senha: " + body);

  int code = http.POST(body);
  if (code <= 0) {
    Serial.println("❌ Falha HTTP");
    return -1;
  }

  String resposta = http.getString();
  Serial.println("📡 Resposta validação: " + resposta);

  http.end();

  if (resposta.indexOf("\"valid\":1") >= 0) return 1;
  return 0;
}

// ======================================================
// ================ FEEDBACK DE VIBRAÇÃO ===============
// ======================================================
void vibrarCurto() {
  digitalWrite(pinVibracao, LOW);
  delay(1000);
  digitalWrite(pinVibracao, HIGH);
}

void vibrarLongo() {
  digitalWrite(pinVibracao, LOW);
  delay(3000);
  digitalWrite(pinVibracao, HIGH);
}

// ======================================================
// ======================== SETUP ========================
// ======================================================
void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(pinVibracao, OUTPUT);
  digitalWrite(pinVibracao, HIGH);

  Serial.println("=== Sistema IoT ESP32 ===");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nWiFi OK!");
  Serial.print("IP: "); Serial.println(WiFi.localIP());

  // Inicializar DS18B20 apenas para ESP3
  if (strcmp(SENSOR_MODE, "temperatura") == 0)
    sensors.begin();

  // Inicializar modo ESP2 (Relé)
  if (strcmp(SENSOR_MODE, "esp2") == 0) {
      pinMode(pinRele, OUTPUT);
      digitalWrite(pinRele, LOW); // começa apagado
  }


  // =============================================
  // Inicializar LEDs SOMENTE para o modo ESP4
  // =============================================
  if (strcmp(SENSOR_MODE, "esp4") == 0) {
    pinMode(LED_VERDE, OUTPUT);
    pinMode(LED_AMARELO, OUTPUT);
    pinMode(LED_VERMELHO, OUTPUT);

    apagarTodosLeds();
  }

  Serial.print("Modo: "); Serial.println(SENSOR_MODE);
}


// ======================================================
// ========================= LOOP ========================
// ======================================================
void loop() {

  if (strcmp(SENSOR_MODE, "esp1") == 0) loopESP1();
  else if (strcmp(SENSOR_MODE, "esp2") == 0) loopESP2();
  else if (strcmp(SENSOR_MODE, "esp3") == 0) loopESP3();
  else if (strcmp(SENSOR_MODE, "esp4") == 0) loopESP4();
  else if (strcmp(SENSOR_MODE, "teclado") == 0) lerTeclado();
  else if (strcmp(SENSOR_MODE, "cor") == 0) lerCor();
  else if (strcmp(SENSOR_MODE, "gestos") == 0) lerGestos();
  else if (strcmp(SENSOR_MODE, "joystick") == 0) lerJoystick();
  else if (strcmp(SENSOR_MODE, "umidade") == 0) lerDHT();
  else if (strcmp(SENSOR_MODE, "velocidade") == 0) lerVelocidade();
  else if (strcmp(SENSOR_MODE, "rele") == 0) controlarRele();
  else if (strcmp(SENSOR_MODE, "vibracao") == 0) controlarVibracao();
  else if (strcmp(SENSOR_MODE, "temperatura") == 0) lerTemperatura();
}

// ======================================================
// ====================== ESP1 ==========================
// ======================================================
void loopESP1() {

  char tecla = teclado.getKey();
  if (!tecla) return;

  Serial.printf("[ESP1] Tecla: %c\n", tecla);

  if (tecla == '*') {
    senhaDigitada = "*";
    capturando = true;
    return;
  }

  if (!capturando) return;

  senhaDigitada += tecla;

  if (senhaDigitada.length() == 5) {

    int resultado = validarSenha(senhaDigitada);

    // ===== REGISTRAR ESTADO NO /readings =====
    if (resultado == 1) {
      Serial.println("✔ ESP1 — Senha correta!");

      String json = "{";
      json += "\"componentId\":\"" + ID_TECLADO + "\",";
      json += "\"data\":\"{\\\"status\\\":\\\"acesso_liberado\\\"}\"";
      json += "}";
      enviarParaAPI(json);

      vibrarCurto();

    } else {
      Serial.println("❌ ESP1 — Senha incorreta!");

      String json = "{";
      json += "\"componentId\":\"" + ID_TECLADO + "\",";
      json += "\"data\":\"{\\\"status\\\":\\\"acesso_negado\\\"}\"";
      json += "}";
      enviarParaAPI(json);

      vibrarLongo();
    }

    senhaDigitada = "";
    capturando = false;
  }
}

String pegarUltimoRegistroComponente(String json, String componentId) {

  int pos = json.lastIndexOf(componentId);
  if (pos == -1) return "";

  int inicio = json.lastIndexOf("{", pos);
  if (inicio == -1) return "";

  int fim = inicio;
  int nivel = 0;

  for (int i = inicio; i < json.length(); i++) {
    char c = json[i];

    if (c == '{') nivel++;
    if (c == '}') nivel--;

    if (nivel == 0) {
      fim = i;
      break;
    }
  }

  String obj = json.substring(inicio, fim + 1);

  // Corrige aspas quebradas da API
  obj.replace("\\\"", "\"");

  return obj;
}



// ======================================================
// FUNÇÃO PARA EXTRAIR TEMPERATURA DE QUALQUER FORMATO
// ======================================================
float extrairTemperatura(String jsonObj) {

  // ----- Caso 1: formato {"t": 33.9, "h": ...}
  int tIndex = jsonObj.indexOf("\"t\"");
  if (tIndex >= 0) {
    int start = jsonObj.indexOf(":", tIndex) + 1;
    int end = jsonObj.indexOf(",", start);
    if (end == -1) end = jsonObj.indexOf("}", start);
    return jsonObj.substring(start, end).toFloat();
  }

  // ----- Caso 2: formato {"temperature": 22.5, ...}
  int tempIndex = jsonObj.indexOf("\"temperature\"");
  if (tempIndex >= 0) {
    int start = jsonObj.indexOf(":", tempIndex) + 1;
    int end = jsonObj.indexOf(",", start);
    if (end == -1) end = jsonObj.indexOf("}", start);
    return jsonObj.substring(start, end).toFloat();
  }

  return 0.0;
}


// ======================================================
// ====================== ESP2 ==========================
// ===== Relé + Encoder (porta) =========================
// ======================================================
void loopESP2() {

  HTTPClient http;

  // 1) Verificar acesso
  String urlStatus = "https://api-cogr.onrender.com/readings?componentId=" + ID_TECLADO;
  http.begin(urlStatus);
  http.GET();
  String resp = http.getString();
  http.end();

  Serial.println("📡 ESP2 RAW:");
  Serial.println(resp);

  String lastAcc = pegarUltimoRegistroComponente(resp, ID_TECLADO);
  bool acessoLiberado = lastAcc.indexOf("acesso_liberado") >= 0;
  bool acessoNegado   = lastAcc.indexOf("acesso_negado") >= 0;


  // ====================================================
  // RELÉ (usa pinRele - mesmo da função controlarRele)
  // ====================================================
  if (acessoLiberado) {
    Serial.println("🟢 RELÉ ATIVADO");
    digitalWrite(pinRele, HIGH);  // ativa relé (invertido)
  }
  else if (acessoNegado) {
    Serial.println("🔴 RELÉ DESATIVADO");
    digitalWrite(pinRele, LOW);
  }


  // ====================================================
  // === ENCODER — mesmo comportamento de lerVelocidade ===
  // ====================================================
  int leitura = digitalRead(sensorSpeedPin);

  if (leitura == HIGH) {

    // Pulso detectado — exatamente igual ao lerVelocidade()
    countDeteccao++;

    String json = "{";
    json += "\"componentId\":\"" + ID_VELOCIDADE + "\",";
    json += "\"value\":\"" + String(countDeteccao) + "\"";
    json += "}";
    enviarParaAPI(json);

    delay(300);  // mesmo delay do lerVelocidade()

    // Consideramos porta aberta durante pulsos
    if (!portaEstaAberta) {
      portaEstaAberta = true;
      portaAbertaDesde = millis();
      Serial.println("🚪 Porta ABERTA (pelo encoder)");
    }
  }
  else {
    // Se não há pulsos e antes havia — porta fechada
    if (portaEstaAberta) {
      portaEstaAberta = false;
      Serial.println("🚪 Porta FECHADA (encoder parou)");
    }
  }


  // ====================================================
  // ALERTA SE PORTA ABERTA POR >5s
  // ====================================================
  if (portaEstaAberta && (millis() - portaAbertaDesde > 5000)) {
    Serial.println("⚠️ Porta aberta por tempo excessivo!");
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_VERMELHO, HIGH);
  }

  delay(200);
}



// ======================================================
// ====================== ESP3 ==========================
// ====== Temperatura e Umidade (DHT22) =================
// ======================================================
// ======================================================
// ====================== ESP3 ==========================
// ====== Temperatura e Umidade (DHT22) =================
// ======================================================
void loopESP3() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  Serial.print("[ESP3] TEMP: ");
  Serial.print(t);
  Serial.print("°C / UMIDADE: ");
  Serial.println(h);

  // Enviar leitura normal do sensor
  String json = "{";
  json += "\"componentId\":\"" + ID_TEMP_DS18B20 + "\",";
  json += "\"data\":\"{\\\"t\\\":" + String(t) + ",\\\"h\\\":" + String(h) + "}\"";
  json += "}";
  enviarParaAPI(json);

  // Limite no Arduino
  const float LIMITE = 30.0;

  if (t > LIMITE) {
    Serial.println("🔥 ESP3 — Temperatura alta! Registrando em /readings...");

    String estado = "{";
    estado += "\"componentId\":\"" + ID_TEMP_DS18B20 + "\",";
    estado += "\"data\":\"{\\\"status\\\":\\\"temp_alta\\\"}\"";
    estado += "}";
    enviarParaAPI(estado);
  }

  delay(1000);
}



// ======================================================
// ====================== ESP4 ==========================
// ========== Indicador com LEDs (verde, amarelo, vermelho)
// ======================================================
// ======================================================
// ====================== ESP4 ==========================
// ========== Indicador com LEDs (verde, amarelo, vermelho)
// ======================================================
// ---------------------------------------------------------
// Função auxiliar para extrair o último objeto de um array
// ---------------------------------------------------------
String extrairUltimoObjeto(String json) {

  int fim = json.lastIndexOf("}");        // último }
  int separador = json.lastIndexOf("},{"); // última fronteira },{

  // início deve pegar o '{' do último item
  int inicio = separador + 2;

  return json.substring(inicio, fim + 1);
}

// ---------------------------------------------------------
// LOOP ESP4 - LEDS DE AVALIAÇÃO
// ---------------------------------------------------------
// ==========================================================
// FUNÇÃO PARA PEGAR O ÚLTIMO REGISTRO DO COMPONENTE CORRETO
// ==========================================================

// ======================================================
// ====================== ESP4 ==========================
// ========== Indicador com LEDs (verde, amarelo, vermelho)
// ======================================================
// ==========================================================
// FUNÇÃO PARA PEGAR O ÚLTIMO REGISTRO DO COMPONENTE CORRETO
// ==========================================================
// ==========================================================
// FUNÇÃO PARA PEGAR O ÚLTIMO REGISTRO DO COMPONENTE CORRETO
// ==========================================================




// ======================================================
// ====================== ESP4 LOOP =====================
// ======================================================
void loopESP4() {

  apagarTodosLeds();

  HTTPClient http;

  // ===========================================================
  // 1) TEMPERATURA
  // ===========================================================
  String urlTemp = "https://api-cogr.onrender.com/readings?componentId=" + ID_TEMP_DS18B20;

  http.begin(urlTemp);
  http.GET();
  String respTemp = http.getString();
  http.end();

  Serial.println("📡 ESP4 TEMP RAW:");
  Serial.println(respTemp);

  String lastTempEntry = pegarUltimoRegistroComponente(respTemp, ID_TEMP_DS18B20);

  Serial.println("📦 Última entrada TEMP filtrada:");
  Serial.println(lastTempEntry);

  float temperatura = extrairTemperatura(lastTempEntry);

  Serial.print("🌡 Temp atual: ");
  Serial.println(temperatura);

  if (temperatura > 30.0) {
    Serial.println("🟡 ALERTA: Temperatura alta!");
    digitalWrite(LED_AMARELO, HIGH);
  }



  // ===========================================================
  // 2) STATUS DO TECLADO
  // ===========================================================
  String urlAcc = "https://api-cogr.onrender.com/readings?componentId=" + ID_TECLADO;

  http.begin(urlAcc);
  http.GET();
  String respAcc = http.getString();
  http.end();

  Serial.println("📡 ESP4 ACESSO RAW:");
  Serial.println(respAcc);

  String lastAccEntry = pegarUltimoRegistroComponente(respAcc, ID_TECLADO);

  Serial.println("📦 Última entrada ACESSO filtrada:");
  Serial.println(lastAccEntry);

  bool acessoLiberado = lastAccEntry.indexOf("acesso_liberado") >= 0;
  bool acessoNegado   = lastAccEntry.indexOf("acesso_negado") >= 0;

  if (acessoNegado) {
    Serial.println("🔴 ESP4 → ACESSO NEGADO");
    digitalWrite(LED_VERMELHO, HIGH);
  }
  else if (acessoLiberado) {
    Serial.println("🟢 ESP4 → ACESSO LIBERADO");
    digitalWrite(LED_VERDE, HIGH);
  }

  delay(1000);
}






// ======================================================
// ==================== SENSORES ========================
// ======================================================

// === TECLADO ===
void lerTeclado() {
  char tecla = teclado.getKey();
  if (tecla) {
    Serial.printf("[TECLA] %c\n", tecla);

    String json = "{";
    json += "\"componentId\":\"" + ID_TECLADO + "\",";
    json += "\"value\":\"" + String(tecla) + "\"";
    json += "}";

    enviarParaAPI(json);
  }
}

// === COR (APDS) ===
void lerCor() {
  apds.readAmbientLight(ambient_light);
  apds.readRedLight(red_light);
  apds.readGreenLight(green_light);
  apds.readBlueLight(blue_light);

  String value =
    String("{\"r\":") + red_light +
    ",\"g\":" + green_light +
    ",\"b\":" + blue_light + "}";

  String json = "{";
  json += "\"componentId\":\"" + ID_APDS9960 + "\",";
  json += "\"value\":\"" + value + "\"";
  json += "}";

  enviarParaAPI(json);
}

// === GESTOS ===
void lerGestos() {
  if (apds.isGestureAvailable()) {
    int g = apds.readGesture();
    String gesto = "UNKNOWN";
    if (g == DIR_UP) gesto = "UP";
    if (g == DIR_DOWN) gesto = "DOWN";
    if (g == DIR_LEFT) gesto = "LEFT";
    if (g == DIR_RIGHT) gesto = "RIGHT";

    String json = "{";
    json += "\"componentId\":\"" + ID_APDS9960 + "\",";
    json += "\"value\":\"" + gesto + "\"";
    json += "}";

    enviarParaAPI(json);
  }
}

// === JOYSTICK ===
void lerJoystick() {
  int x = map(analogRead(sensorPinX), 0, 4095, -128, 127);
  int y = map(analogRead(sensorPinY), 0, 4095, -128, 127);

  String json = "{";
  json += "\"componentId\":\"" + ID_JOYSTICK + "\",";
  json += "\"value\":\"{\\\"x\\\":" + String(x) + ",\\\"y\\\":" + String(y) + "}\"";
  json += "}";

  enviarParaAPI(json);
}

// === DHT22 ===
void lerDHT() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  String payload = "{\"t\":" + String(t) + ",\"h\":" + String(h) + "}";

  String json = "{";
  json += "\"componentId\":\"" + ID_TEMP_DS18B20 + "\",";
  json += "\"value\":\"" + payload + "\"";
  json += "}";

  enviarParaAPI(json);
}

// === VELOCIDADE ===
void lerVelocidade() {
  if (digitalRead(sensorSpeedPin) == HIGH) {
    countDeteccao++;

    String json = "{";
    json += "\"componentId\":\"" + ID_VELOCIDADE + "\",";
    json += "\"value\":\"" + String(countDeteccao) + "\"";
    json += "}";

    enviarParaAPI(json);

    delay(300);
  }
}

// === RELÉ ===
void controlarRele() {
  digitalWrite(pinRele, LOW);

  String json = "{\"componentId\":\"" + ID_RELE + "\",\"value\":\"ON\"}";
  enviarParaAPI(json);

  delay(2000);

  digitalWrite(pinRele, HIGH);
  json = "{\"componentId\":\"" + ID_RELE + "\",\"value\":\"OFF\"}";
  enviarParaAPI(json);

  delay(2000);
}

// === VIBRAÇÃO ===
void controlarVibracao() {
  digitalWrite(pinVibracao, LOW);
  enviarParaAPI("{\"componentId\":\"" + ID_SERVO + "\",\"value\":\"ON\"}");
  delay(2000);

  digitalWrite(pinVibracao, HIGH);
  enviarParaAPI("{\"componentId\":\"" + ID_SERVO + "\",\"value\":\"OFF\"}");
  delay(2000);
}

// === DS18B20 ===
void lerTemperatura() {
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);

  String json = "{";
  json += "\"componentId\":\"" + ID_TEMP_DS18B20 + "\",";
  json += "\"value\":\"" + String(temp) + "\"";
  json += "}";

  enviarParaAPI(json);
}
