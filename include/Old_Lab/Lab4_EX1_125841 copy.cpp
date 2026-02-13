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
void step1();

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
            Serial.println("SW1 Press 1: Read LDR Value");
            break;
          case 1:
            digitalWrite(LED_BUILTIN, LOW);
            Serial.println("SW1 Press 2: LED ON");
            break;
          case 2:
            digitalWrite(LED_BUILTIN, HIGH);
            Serial.println("SW1 Press 3: LED OFF");
            break;
          case 3:
            digitalWrite(LED_BUILTIN, LOW);
            Serial.println("SW1 Press 4: LED ON");
            break;
          case 4:
            digitalWrite(LED_BUILTIN, HIGH);
            Serial.println("SW1 Press 5: LED OFF");
            break;
          case 5:
            digitalWrite(LED_BUILTIN, LOW);
            Serial.println("SW1 Press 6: LED ON");
            break;
        }
        sw1State = (sw1State + 1) % 6;
      }
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
  return 
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
  Serial.println("Temperature: "+String(temperature, 2)+" C, Humidity: "+String(humidity, 2)+" %");
}