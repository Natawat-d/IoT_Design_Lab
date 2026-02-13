#include <Arduino.h>
#include <DHT.h>

#define LED_BUILTIN 2
#define SW1 18
#define LDR_PIN 34 
#define DHTPIN 32
#define DHTTYPE DHT22

bool lastSW1State = LOW;
unsigned long lastSW1PressTime = 0;
const unsigned long DEBOUNCE_DELAY = 200;
int sw1State = 0;

unsigned long previousLEDMillis = 0;
unsigned long previousSensorMillis = 0;
const unsigned long LED_BLINK_INTERVAL = 500;
const unsigned long SENSOR_CHECK_INTERVAL = 1000;
bool ledState = false;

DHT dht(DHTPIN, DHTTYPE);

void setup() 
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SW1, INPUT_PULLUP);
  digitalWrite(LED_BUILTIN, HIGH);
  dht.begin();
  Serial.println("System ready.");
}

float adcToLux(int ldrPin);
void readDHT(float &temp, float &h, bool TempType);
void step1();
void step2();
void step3();
void step4();

void loop() 
{
  unsigned long currentMillis = millis();
  bool sw1Current = digitalRead(SW1);
  
  if (sw1Current != lastSW1State) 
  {
    if (currentMillis - lastSW1PressTime >= DEBOUNCE_DELAY) 
    {
      lastSW1State = sw1Current;
      lastSW1PressTime = currentMillis;
      
      if (!sw1Current) {
       
        switch (sw1State) {
          case 0:
            step1();
            break;
          case 1:
            step2();
            break;
          case 2:
            step3();
            break;
          case 3:
            step4();
            break;
        }
        sw1State = (sw1State + 1) % 4;
      }
    }
  }
  
  // Continuous sensor monitoring and LED control
  if (currentMillis - previousSensorMillis >= SENSOR_CHECK_INTERVAL) {
    previousSensorMillis = currentMillis;
    
    float lux = adcToLux(LDR_PIN);
    float temperature, humidity;
    readDHT(temperature, humidity, false);
    
    // Control LED based on LDR (LDR > 150: LED ON, < 150: LED OFF)
    if (lux > 150) {
      // Check if temperature > 25C for flashing
      if (temperature > 25) {
        // LED will flash - handled by blink logic below
      } else {
        digitalWrite(LED_BUILTIN, LOW); // LED ON solid
      }
    } else {
      digitalWrite(LED_BUILTIN, HIGH); // LED OFF
    }
  }
  
  // LED flashing logic when temperature > 25C and lux > 150
  float temperature, humidity;
  readDHT(temperature, humidity, false);
  float lux = adcToLux(LDR_PIN);
  
  if (temperature > 25 && lux > 150) {
    if (currentMillis - previousLEDMillis >= LED_BLINK_INTERVAL) {
      previousLEDMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
    }
  }
}


__inline__ float adcToLux(int ldrPin) {
   int adcValue = analogRead(ldrPin);
  // Adapted Formula: lux = (250 / (ADC_Step * raw_val)) - 50
  // ADC_Step for ESP32 (3.3V / 4095) is approx 0.00080586
  float ADC_value = 0.0008058608f; 
  
  if (adcValue <= 0) return 0.0; // Avoid division by zero or negative
  
  return (250.000000 / (ADC_value * adcValue)) - 50.000000;
}

__inline__ void readDHT(float &temp, float &h, bool TempType) {
  temp = dht.readTemperature(TempType);  // TempType: false = Celsius, true = Fahrenheit
  h = dht.readHumidity();
}


__inline__ void step1()
{
  float lux = adcToLux(LDR_PIN);
  Serial.println("Light Value in the Room = "+String(lux, 2)+" lux");
}

__inline__ void step2()
{
  float temperature, humidity;
  readDHT(temperature, humidity, false);
  Serial.println("Humidity Value in the Room = "+String(humidity, 2)+" %H");
}

__inline__ void step3()
{
  float temperature, humidity;
  readDHT(temperature, humidity, false);
  Serial.println("Temperature Value in the Room = "+String(temperature, 2)+" °C");
}

__inline__ void step4()
{
  float temperature, humidity;
  readDHT(temperature, humidity, true);
  Serial.println("Temperature Value in the Room = "+String(temperature, 2)+" °F");
}