#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define LDR_PIN 34 
#define DHTPIN 32
#define DHTTYPE DHT22
#define SDA_PIN 21
#define SCL_PIN 22
#define SW1 18
#define LED_BUILTIN 2

LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 columns, 2 rows
DHT dht(DHTPIN, DHTTYPE);

bool lastSW1State = HIGH;
unsigned long lastSW1PressTime = 0;
const unsigned long DEBOUNCE_DELAY = 200;
int displayState = 0; // 0-3 for 4 states

unsigned long previousLCDMillis = 0;
const unsigned long LCD_UPDATE_INTERVAL = 100; // Update every 1 second

__inline__ float adcToLux(int ldrPin);
__inline__ void readDHT(float &temp, float &h, bool TempType);
__inline__ void ClearLCDsecondline();

void setup() 
{
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);  // Initialize I2C with SDA=21, SCL=22
  lcd.init();
  lcd.backlight();
  dht.begin();
  
  pinMode(SW1, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // LED OFF initially
  
  lcd.setCursor(0, 0);
  lcd.print("Train by I-Kit");
  ClearLCDsecondline();
  
  Serial.println("System ready.");
}



void loop() 
{
  unsigned long currentMillis = millis();
  bool sw1Current = digitalRead(SW1);
  
  // LED follows button state: ON when pressed (LOW), OFF when released (HIGH)
  if (!sw1Current) {
    digitalWrite(LED_BUILTIN, HIGH);  // LED ON when button pressed
  } else {
    digitalWrite(LED_BUILTIN, LOW); // LED OFF when button released
  }
  
  // Check for button press with debounce
  if (sw1Current != lastSW1State) {
    if (currentMillis - lastSW1PressTime >= DEBOUNCE_DELAY) {
      lastSW1State = sw1Current;
      lastSW1PressTime = currentMillis;
      
      if (!sw1Current) { // Button pressed (LOW)
        displayState = (displayState + 1) % 4; // Cycle through 0-3
        previousLCDMillis = currentMillis; // Force immediate update
      }
    }
  }

  // Update LCD based on timing interval
  if (currentMillis - previousLCDMillis >= LCD_UPDATE_INTERVAL) {
    previousLCDMillis = currentMillis;
    
    float temperature, humidity;
    float lux;
          
    switch (displayState) {
      case 0: // Temperature in Celsius
        
        readDHT(temperature, humidity, false);
        ClearLCDsecondline();
        lcd.print("Temperature=");
        lcd.print(temperature, 1);
        lcd.print((char)223); // degree symbol
        lcd.print("C");
        break;

      case 1: // Humidity
        readDHT(temperature, humidity, false);
        ClearLCDsecondline();
        lcd.print("Humidity = ");
        lcd.print(humidity, 1);
        lcd.print(" %H");
        break;

      case 2: // Temperature in Fahrenheit
        readDHT(temperature, humidity, true);
        ClearLCDsecondline();
        lcd.print("Temperature=");
        lcd.print(temperature, 1);
        lcd.print((char)223); // degree symbol
        lcd.print("F");
        break;

      case 3: // Light
        lux = adcToLux(LDR_PIN);
        ClearLCDsecondline();
        lcd.print("Light = ");
        lcd.print(lux, 0);
        lcd.print(" lux");
        break;
    }
  }
}

__inline__ void ClearLCDsecondline() {
  lcd.setCursor(0, 1);
  lcd.print("                "); // Clear line
  lcd.setCursor(0, 1);
  
}
__inline__ float adcToLux(int ldrPin) {
  int adcValue = analogRead(ldrPin);
  float ADC_value = 0.0008058608f; 
  
  if (adcValue <= 0) return 0.0;
  
  return (250.000000 / (ADC_value * adcValue)) - 50.000000;
}

__inline__ void readDHT(float &temp, float &h, bool TempType) {
  temp = dht.readTemperature(TempType);  // TempType: false = Celsius, true = Fahrenheit
  h = dht.readHumidity();
}
