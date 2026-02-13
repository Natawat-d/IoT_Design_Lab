#include <Arduino.h>

volatile uint32_t* gpio_in_reg = (volatile uint32_t*)0x3FF4403C;

#define BUTTON_PIN 17 // Safe general purpose I/O pin

bool toggleState = false;

void setup() {
  // Initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  // Read button state using direct register access
  // LOW (0) means pressed because of INPUT_PULLUP
  bool isPressed = ((*gpio_in_reg & (1 << BUTTON_PIN)) == 0);

  if (isPressed) {
    toggleState = !toggleState; // Toggle the state
    
    Serial.print("Toggle: ");
    Serial.println(toggleState ? "ON" : "OFF");
    
    delay(50); // Debounce
    
    // Wait until button is released to prevent multiple toggles
    while ((*gpio_in_reg & (1 << BUTTON_PIN)) == 0) {
      delay(10);
    }
  }
}