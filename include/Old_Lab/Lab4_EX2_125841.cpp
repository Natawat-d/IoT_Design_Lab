#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define LDR_PIN 34 
#define DHTPIN 32
#define DHTTYPE DHT22
#define SDA_PIN 21
#define SCL_PIN 22

LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 columns, 2 rows
DHT dht(DHTPIN, DHTTYPE);

unsigned long previousMillis = 0;
const unsigned long DISPLAY_INTERVAL = 10000; // 10 seconds
int displayState = 0; // 0-3 for 4 states

unsigned long previousLCDMillis = 0;
const unsigned long LCD_UPDATE_INTERVAL = 500; // Update LCD every 1 second

void setup() 
{
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);  // Initialize I2C with SDA=21, SCL=22
  lcd.init();
  lcd.backlight();
  dht.begin();
  
  lcd.setCursor(0, 0);
  lcd.print("IoT Training");
  
  Serial.println("System ready.");
}

float adcToLux(int ldrPin);
void readDHT(float &temp, float &h, bool TempType);

void loop() 
{
  unsigned long currentMillis = millis();
  
  // Check if 10 seconds have passed
  if (currentMillis - previousMillis >= DISPLAY_INTERVAL) {
    previousMillis = currentMillis;
    displayState = (displayState + 1) % 4; // Cycle through 0-3
    previousLCDMillis = currentMillis; // Force immediate update
  }
  
  // Update LCD display based on timing interval
  if (currentMillis - previousLCDMillis >= LCD_UPDATE_INTERVAL) {
    previousLCDMillis = currentMillis;
    
    // Update second line based on current state
    lcd.setCursor(0, 1);
    lcd.print("                "); // Clear line
    lcd.setCursor(0, 1);
    
    float temperature, humidity;
    float lux;
    
    switch (displayState) {
      case 0: // Temperature in Celsius
        readDHT(temperature, humidity, false);
        lcd.print("Temperature=");
        lcd.print(temperature, 1);
        lcd.print((char)223); // degree symbol
        lcd.print("C");
        break;
        
      case 1: // Humidity
        readDHT(temperature, humidity, false);
        lcd.print("Humidity = ");
        lcd.print(humidity, 1);
        lcd.print(" %H");
        break;
        
      case 2: // Temperature in Fahrenheit
        readDHT(temperature, humidity, true);
        lcd.print("Temperature=");
        lcd.print(temperature, 1);
        lcd.print((char)223); // degree symbol
        lcd.print("F");
        break;
        
      case 3: // Light
        lux = adcToLux(LDR_PIN);
        lcd.print("Light = ");
        lcd.print(lux, 0);
        lcd.print(" lux");
        break;
    }
  }
}

float adcToLux(int ldrPin) {
  int adcValue = analogRead(ldrPin);
  float ADC_value = 0.0008058608f; 
  
  if (adcValue <= 0) return 0.0;
  
  return (250.000000 / (ADC_value * adcValue)) - 50.000000;
}

void readDHT(float &temp, float &h, bool TempType) {
  temp = dht.readTemperature(TempType);  // TempType: false = Celsius, true = Fahrenheit
  h = dht.readHumidity();
}
