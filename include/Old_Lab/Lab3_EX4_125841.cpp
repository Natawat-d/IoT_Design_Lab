#include <Arduino.h>

// Pin definitions
#define LED1 4              // External LED1 connected to GPIO 4
#define LED_BUILTIN 2       // ESP32 built-in LED on GPIO 2 (active LOW)
#define LDR_PIN 34          // LDR sensor connected to ADC pin (GPIO 34)

// Timing variables (non-blocking)
unsigned long previousLED1Millis = 0;
unsigned long previousLDRMillis = 0;

// Timing intervals
const unsigned long LED1_INTERVAL = 2000; // 2 seconds for LED1 blink
const unsigned long LDR_INTERVAL = 100;   // 100ms for LDR reading

// State variables
bool startupComplete = false;
bool led1State = LOW;

// Function declaration
__inline__ float adcToLux(int adcValue);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Initialize LED pins as output
  pinMode(LED1, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Startup blink sequence - both LEDs blink 3 times
  Serial.println("Starting up...");
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED_BUILTIN, LOW);   // Active LOW - turn ON
    delay(500);
    digitalWrite(LED1, LOW);
    digitalWrite(LED_BUILTIN, HIGH);  // Active LOW - turn OFF
    delay(500);
  }
  
  startupComplete = true;
  Serial.println("Startup complete.");
  Serial.println("LED1: Status indicator (2s blink)");
  Serial.println("Built-in LED: LDR controlled (threshold: 150 lux)");
  
  // Initialize timing
  previousLED1Millis = millis();
  previousLDRMillis = millis();
}

void loop() {
  if (startupComplete) {
    unsigned long currentMillis = millis();
    
    // Handle LED1 blinking every 2 seconds (status indicator)
    if (currentMillis - previousLED1Millis >= LED1_INTERVAL) {
      previousLED1Millis = currentMillis;
      led1State = !led1State;
      digitalWrite(LED1, led1State);
    }
    
    // Read LDR sensor and control built-in LED every 100ms
    if (currentMillis - previousLDRMillis >= LDR_INTERVAL) {
      previousLDRMillis = currentMillis;
      
      int adcValue = analogRead(LDR_PIN);
      float luxValue = adcToLux(adcValue);
      
      // Print LDR value to serial monitor
      Serial.print("ADC: ");
      Serial.print(adcValue);
      Serial.print(" | Lux: ");
      Serial.print(luxValue, 2);
      Serial.print(" | LED1: ");
      Serial.print(led1State ? "ON" : "OFF");
      Serial.print(" | Built-in LED: ");
      
      // Control built-in LED based on lux value (active LOW)
      if (luxValue > 150.0) {
        digitalWrite(LED_BUILTIN, LOW);   // Active LOW - turn ON
        Serial.println("ON");
      } else {
        digitalWrite(LED_BUILTIN, HIGH);  // Active LOW - turn OFF
        Serial.println("OFF");
      }
    }
  }
  
  // No delay - continuous non-blocking operation
}

// ADC to Lux conversion function
__inline__ float adcToLux(int adcValue) {
  // Adapted Formula: lux = (250 / (ADC_Step * raw_val)) - 50
  // ADC_Step for ESP32 (3.3V / 4095) is approx 0.00080586
  float ADC_value = 0.0008058608f; 
  
  if (adcValue <= 0) return 0.0; // Avoid division by zero or negative
  
  return (250.000000 / (ADC_value * adcValue)) - 50.000000;
}
