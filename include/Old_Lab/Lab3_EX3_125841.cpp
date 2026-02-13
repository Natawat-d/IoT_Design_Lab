#include <Arduino.h>

// Pin definitions
#define LED1 2        // LED connected to GPIO 2
#define LDR_PIN 34    // LDR sensor connected to ADC pin (GPIO 34)



__inline__ float adcToLux(int adcValue) ;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Initialize LED pin as output
  pinMode(LED1, OUTPUT);
  
  // Blink LED 3 times at startup
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED1, HIGH);
    delay(500);
    digitalWrite(LED1, LOW);
    delay(500);
  }
  
  
}

void loop() {
  // Read LDR sensor value
  float lux_value = adcToLux(analogRead(LDR_PIN));
  
  // Print LDR value to serial monitor
  Serial.print("lux Value: ");
  Serial.println(lux_value);
  
  // Control LED based on LDR value
  if (lux_value > 150.0) {
    digitalWrite(LED1, HIGH);  // Turn LED on
  } else {
    digitalWrite(LED1, LOW);   // Turn LED off
  }
  
  delay(100);  // Small delay for stability
}

__inline__ float adcToLux(int adcValue) {
  // Adapted Formula: lux = (250 / (ADC_Step * raw_val)) - 50
  // ADC_Step for ESP32 (3.3V / 4095) is approx 0.00080586
  float ADC_value = 0.0008058608f; 
  
  if (adcValue <= 0) return 0.0; // Avoid division by zero or negative
  
  return (250.000000 / (ADC_value * adcValue)) - 50.000000;
}