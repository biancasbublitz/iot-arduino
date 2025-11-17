# 🧠 Guia de Utilização – Sistema Unificado de Sensores IoT (ESP32)

Este projeto reúne múltiplos sensores e atuadores em um único código para ESP32, podendo alternar entre eles definindo o `SENSOR_MODE`.

---

## ⚙️ 1️⃣ Selecionando o Sensor/Atuador

No início do código, altere:

```cpp
#define SENSOR_MODE "..."
```

### 🔧 Valores possíveis:

| Valor possível  | Comportamento / Dispositivo |
| --------------- | --------------------------- |
| `"cor"`         | Sensor de cor APDS9960      |
| `"gestos"`      | Sensor de gestos APDS9960   |
| `"distancia"`   | Sensor ultrassônico         |
| `"joystick"`    | Joystick analógico          |
| `"teclado"`     | Teclado matricial 4x4       |
| `"umidade"`     | Sensor DHT22                |
| `"velocidade"`  | Sensor de rotações          |
| `"rele"`        | Módulo relé                 |
| `"temperatura"` | Sensor DS18B20              |

---

## 🧪 1.1 – Modos especiais de simulação do trabalho (ESP1, ESP2, ESP3, ESP4)

Além dos sensores individuais, você também pode configurar:

| Modo    | Função simulada                                                                 |
| -------- | ---------------------------------------------------------------------------- |
| `"esp1"` | Leitura do **teclado**, envio de senha e validação via API                   |
| `"esp2"` | Controle do **relé** + leitura do **encoder** simulando abertura de porta     |
| `"esp3"` | Leitura de **temperatura e umidade** (DHT22) + alerta de temperatura          |
| `"esp4"` | Indicador de status com **LEDs** (verde, amarelo, vermelho) baseado na API    |

Esses modos são exigidos no trabalho e simulam fluxos reais do sistema.

---

# 🔌 2️⃣ Ligações dos Sensores

Abaixo estão **TODAS as portas conferidas diretamente no seu código**.

---

## 🌈 2.1 – Sensor de Cor – APDS9960

| Pino do sensor | ESP32  |
| -------------- | ------ |
| SDA            | GPIO21 |
| SCL            | GPIO22 |
| VCC            | 3V3    |
| GND            | GND    |

---

## ✋ 2.2 – Sensor de Gestos – APDS9960

| Pino do sensor | ESP32  |
| -------------- | ------ |
| SDA            | GPIO21 |
| SCL            | GPIO22 |
| INT            | GPIO33 |
| VCC            | 3V3    |
| GND            | GND    |

---

## 📡 2.3 – Sensor Ultrassônico (HC-SR04)

| Pino | ESP32  |
| ---- | ------ |
| TRIG | GPIO12 |
| ECHO | GPIO13 |
| VCC  | 5V     |
| GND  | GND    |

---

## 🎮 2.4 – Joystick Analógico

| Pino | ESP32  |
| ---- | ------ |
| VRx  | GPIO25 |
| VRy  | GPIO26 |
| SW   | GPIO27 |
| VCC  | 3V3    |
| GND  | GND    |

---

## 🔢 2.5 – Teclado Matricial 4x4

| Tipo    | Pinos usados                | ESP32                                |
| ------- | --------------------------- | ------------------------------------ |
| Linhas  | 4                           | GPIO32, GPIO33, GPIO25, GPIO26       |
| Colunas | 4                           | GPIO27, GPIO17, GPIO12, GPIO13       |
| VCC     | —                           | 3V3                                   |
| GND     | —                           | GND                                   |

---

## 💧 2.6 – Sensor DHT22

| Pino | ESP32  |
| ---- | ------ |
| DATA | GPIO17 |
| VCC  | 3V3    |
| GND  | GND    |

---

## ⚙️ 2.7 – Sensor de Velocidade / Encoder

| Pino | ESP32  |
| ---- | ------ |
| D0   | GPIO21 |
| VCC  | 3V3    |
| GND  | GND    |

---

## 🔌 2.8 – Módulo Relé

| Pino | ESP32  |
| ---- | ------ |
| IN   | GPIO17 |
| VCC  | 5V     |
| GND  | GND    |

---

## 🌡️ 2.9 – Sensor DS18B20

| Pino            | ESP32  |
| ---------------- | ------ |
| DATA             | GPIO22 |
| VCC              | 3V3    |
| GND              | GND    |
| Resistor 4.7kΩ   | Entre **GPIO22 e 3V3** |

---

# 📦 3️⃣ Bibliotecas Necessárias

| Biblioteca                                       | Função            |
| ------------------------------------------------ | ----------------- |
| DHT sensor library                               | Sensor DHT22      |
| Adafruit Unified Sensor                          | Dependência DHT   |
| SparkFun APDS9960 RGB and Gesture Sensor Library | APDS9960          |
| HCSR04 ultrasonic sensor                         | Ultrassônico      |
| Ultrasonic (para ESP32 físico)                   | Ultrassônico real |
| Keypad                                           | Teclado matricial |
| OneWire                                          | DS18B20           |
| DallasTemperature                                | DS18B20           |

---

# 📘 4️⃣ Observações Importantes

- O `SENSOR_MODE` controla qual sensor **ou modo de trabalho** está ativo.
- Nos modos `esp1`, `esp2`, `esp3` e `esp4`, o ESP simula fluxos completos do sistema do trabalho.

