#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Adafruit ESP32 Feather I2C pins
#define I2C_SDA 21
#define I2C_SCL 22

LiquidCrystal_I2C *lcd = nullptr;
byte lcdAddr = 0;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  
  // Initialize I2C with Feather pins
  Wire.begin(I2C_SDA, I2C_SCL);
  
  Serial.println("\n=== I2C Scanner ===");
  Serial.println("Scanning...");
  
  byte count = 0;
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0)
    {
      Serial.print("Found I2C device at: 0x");
      if (i < 16) Serial.print("0");
      Serial.println(i, HEX);
      lcdAddr = i;
      count++;
    }
  }
  Serial.print("Found ");
  Serial.print(count);
  Serial.println(" device(s).");
  
  if (lcdAddr == 0) {
    Serial.println("ERROR: No I2C devices found!");
    Serial.println("Check your wiring:");
    Serial.println("  SDA -> Pin 21");
    Serial.println("  SCL -> Pin 22");
    Serial.println("  VCC -> 5V or 3.3V");
    Serial.println("  GND -> GND");
    return;
  }
  
  // Initialize LCD with detected address
  Serial.print("Initializing LCD at 0x");
  if (lcdAddr < 16) Serial.print("0");
  Serial.println(lcdAddr, HEX);
  
  lcd = new LiquidCrystal_I2C(lcdAddr, 16, 2);
  lcd->init();
  lcd->backlight();
  lcd->setCursor(0, 0);
  lcd->print("LCD Ready!");
  lcd->setCursor(0, 1);
  lcd->print("Addr: 0x");
  lcd->print(lcdAddr, HEX);
  
  Serial.println("LCD initialized successfully!");
}

void loop()
{
  if (lcd == nullptr) {
    delay(1000);
    return;
  }
  
  // when characters arrive over the serial port...
  if (Serial.available()) {
    // wait a bit for the entire message to arrive
    delay(100);
    // clear the screen
    lcd->clear();
    // read all the available characters
    while (Serial.available() > 0) {
      // display each character to the LCD
      lcd->write(Serial.read());
    }
  }
}