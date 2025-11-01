# 🧠 Guia de Utilização – Sistema Unificado de Sensores IoT (ESP32)

IMPORTANTE: FAZER TESTE COM SENSOR DE CORES E GESTOS

OBS: Talvez precise mudar a biblioteca do sensor de distância, faltou o Ultrasonic.h no wokwi

---

## ⚙️ 1️⃣ Selecionando o Sensor/Atuador

No início do código, há a linha:

```cpp
#define SENSOR_MODE "..."
```

Altere o texto entre aspas (`"..."`) para o nome do dispositivo que deseja testar:

| Valor possível  | Dispositivo                          |
| --------------- | ------------------------------------ |
| `"cor"`         | Sensor de cor APDS9960               |
| `"gestos"`      | Sensor de gestos APDS9960            |
| `"distancia"`   | Sensor ultrassônico HC-SR04          |
| `"joystick"`    | Módulo joystick analógico            |
| `"teclado"`     | Teclado matricial 4x4                |
| `"umidade"`     | Sensor DHT22 (umidade e temperatura) |
| `"velocidade"`  | Sensor de detecção de rotações       |
| `"rele"`        | Módulo relé                          |
| `"temperatura"` | Sensor DS18B20                       |

---

## ⚙️ 2️⃣ Pinagem de Conexão por Dispositivo

Abaixo estão os pinos do **ESP32** utilizados em cada modo.\
Sempre conecte o **GND comum** entre o ESP32 e os sensores.

---

### 🌈 **1. Sensor de Cor – APDS9960**

| Pino do sensor | ESP32  | Observação      |
| -------------- | ------ | --------------- |
| SDA            | GPIO21 | Comunicação I²C |
| SCL            | GPIO22 | Comunicação I²C |
| VCC            | 3V3    | Alimentação     |
| GND            | GND    | Terra           |

>

---

### ✋ **2. Sensor de Gestos – APDS9960**

| Pino do sensor | ESP32  | Observação                       |
| -------------- | ------ | -------------------------------- |
| SDA            | GPIO21 | I²C                              |
| SCL            | GPIO22 | I²C                              |
| INT            | GPIO33 | Interrupção para detectar gestos |
| VCC            | 3V3    | Alimentação                      |
| GND            | GND    | Terra                            |

>

---

### 🖯️ **3. Sensor Ultrassônico – HC-SR04**

| Pino do sensor | ESP32  | Observação       |
| -------------- | ------ | ---------------- |
| TRIG           | GPIO12 | Pulso de saída   |
| ECHO           | GPIO13 | Retorno de sinal |
| VCC            | 5V     | Alimentação      |
| GND            | GND    | Terra            |

>

---

### 🎮 **4. Joystick Analógico**

| Pino do módulo | ESP32  | Observação         |
| -------------- | ------ | ------------------ |
| VRx            | GPIO25 | Eixo X (analógico) |
| VRy            | GPIO26 | Eixo Y (analógico) |
| SW             | GPIO27 | Botão (digital)    |
| VCC            | 3V3    | Alimentação        |
| GND            | GND    | Terra              |

>

---

### 🔢 **5. Teclado Matricial 4x4**

| Tipo    | Pinos | ESP32                          |
| ------- | ----- | ------------------------------ |
| Linhas  | 4     | GPIO32, GPIO33, GPIO25, GPIO26 |
| Colunas | 4     | GPIO27, GPIO17, GPIO12, GPIO13 |
| VCC     | 3V3   |                                |
| GND     | GND   |                                |

>

---

### 💧 **6. Sensor DHT22 (Umidade e Temperatura)**

| Pino do sensor | ESP32  | Observação          |
| -------------- | ------ | ------------------- |
| DATA           | GPIO17 | Comunicação digital |
| VCC            | 3V3    | Alimentação         |
| GND            | GND    | Terra               |

>

---

### ⚙️ **7. Sensor de Velocidade / Detecção**

| Pino do sensor     | ESP32  |
| ------------------ | ------ |
| D0 (saída digital) | GPIO21 |
| VCC                | 3V3    |
| GND                | GND    |

>

---

### 🔌 **8. Módulo Relé**

| Pino do módulo | ESP32  | Observação       |
| -------------- | ------ | ---------------- |
| IN             | GPIO17 | Controle do relé |
| VCC            | 5V     | Alimentação      |
| GND            | GND    | Terra            |

>

---

### 🌡️ **9. Sensor de Temperatura DS18B20**

| Pino do sensor | ESP32                  | Observação            |
| -------------- | ---------------------- | --------------------- |
| DATA           | GPIO22                 | Comunicação OneWire   |
| VCC            | 3V3                    | Alimentação           |
| GND            | GND                    | Terra                 |
| Resistor 4.7kΩ | Entre **GPIO22 e 3V3** | Pull-up obrigatório ✅ |

>

---

|   |   |   |
| - | - | - |

---

## 📦 4️⃣ Bibliotecas Necessárias

| Biblioteca                                       | Função            |
| ------------------------------------------------ | ----------------- |
| DHT sensor library                               | Sensor DHT22      |
| Adafruit Unified Sensor                          | Dependência DHT   |
| SparkFun APDS9960 RGB and Gesture Sensor Library | Sensor APDS9960   |
| HCSR04 ultrasonic sensor                         | Sensor HC-SR04    |
| Keypad                                           | Teclado matricial |
| OneWire                                          | DS18B20           |
| DallasTemperature                                | DS18B20           |

>

##

