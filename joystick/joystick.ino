#define sensorPinX 25
#define sensorPinY 26
#define sensorPinButton 27 
	 
#define X_CORRECTION 128 
#define Y_CORRECTION 128 
	 
struct button { 
	byte pressed = 0; 
}; 
	 
struct analog { 
	short x, y; 
	button button; 
}; 
	 
void setup() { 
	pinMode(sensorPinButton, INPUT_PULLUP); 
	Serial.begin(115200); 
} 
	 
void loop() { 
	analog analog; 
	
	analog.x = readAnalogAxisLevel(sensorPinX) - X_CORRECTION; 
	analog.y = readAnalogAxisLevel(sensorPinY) - Y_CORRECTION; 
	
	analog.button.pressed = isAnalogButtonPressed(sensorPinButton); 
	
	Serial.print("X:"); 
	Serial.println(analog.x); 
	
	Serial.print("Y:"); 
	Serial.println(analog.y); 
	
	if (analog.button.pressed) { 
	  Serial.println("Button pressed"); 
	} else { 
	  Serial.println("Button not pressed"); 
	} 
	
	delay(200); 
} 
	 
byte readAnalogAxisLevel(int pin) { 
	return map(analogRead(pin), 0, 1023, 0, 255); 
} 
	 
bool isAnalogButtonPressed(int pin) { 
	return digitalRead(pin) == 0; 
} 
