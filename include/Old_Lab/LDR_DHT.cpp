#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define LDR_PIN 36 // GPIO 36 (VP)
#define DHTPIN 32
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

__inline__ float adcToLux(int adcValue);
__inline__ void readDHT(float &t, float &h);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  dht.begin();
  
  // GPIO 36 is typically input-only, valid for Analog Read
  pinMode(LDR_PIN, INPUT);
}


void loop() {
  // Read the analog value from the LDR sensor (0 - 4095 for 12-bit ADC)
  int ldrValue = analogRead(LDR_PIN);
  
  Serial.print("Lux: ");
  Serial.println(adcToLux(ldrValue));

  float t, h;
  readDHT(t, h);
  Serial.print("Temp: "); Serial.print(t);
  Serial.print(" C, Hum: "); Serial.print(h); Serial.println(" %");
  
  delay(2000); 
}





// Inline function to convert ADC value to Lux (Approximation)
__inline__ float adcToLux(int adcValue) {
  // Adapted Formula: lux = (250 / (ADC_Step * raw_val)) - 50
  // ADC_Step for ESP32 (3.3V / 4095) is approx 0.00080586
  float ADC_value = 0.0008058608f; 
  
  if (adcValue <= 0) return 0.0; // Avoid division by zero or negative
  
  return (250.000000 / (ADC_value * adcValue)) - 50.000000;
}

__inline__ void readDHT(float &t, float &h) {
  t = dht.readTemperature();
  h = dht.readHumidity();
}