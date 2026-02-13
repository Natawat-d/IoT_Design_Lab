#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 32
#define DHTTYPE DHT22

#define I2C_SDA 21
#define I2C_SCL 22

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Initialize LCD (address 0x27, 16 columns, 2 rows)
// Adjust address if needed (common: 0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Function prototypes
inline void readDHTSensor(float &temperature, float &humidity);
inline void displayOnLCD(float temperature, float humidity);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Initialize DHT sensor
  dht.begin();
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Scan for I2C devices
  Serial.println("Scanning I2C bus...");
  byte error, address;
  int nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    }
  }
  if (nDevices == 0) Serial.println("No I2C devices found");
  
  // Display welcome message
  lcd.setCursor(0, 0);
  lcd.print("DHT22 Sensor");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(2000);
}

void loop() {
  float temperature, humidity;
  
  // Read sensor data using inline function
  readDHTSensor(temperature, humidity);
  
  // Display data on LCD using inline function
  displayOnLCD(temperature, humidity);
  
  // Also print to serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  
  delay(2000);
}

// Inline function to read DHT sensor data
inline void readDHTSensor(float &temperature, float &humidity) {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  
  // Check if any reads failed
  if (isnan(temperature) || isnan(humidity)) {
    temperature = -999;
    humidity = -999;
  }
}

// Inline function to display data on LCD
inline void displayOnLCD(float temperature, float humidity) {
  lcd.clear();
  
  // Check for sensor error
  if (temperature == -999 || humidity == -999) {
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!");
    return;
  }
  
  // Display temperature on first row
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature, 1);
  lcd.print(" C");
  
  // Display humidity on second row
  lcd.setCursor(0, 1);
  lcd.print("Hum:  ");
  lcd.print(humidity, 1);
  lcd.print(" %");
}