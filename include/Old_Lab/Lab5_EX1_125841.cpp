#include <WiFi.h>
#include <WiFiManager.h>

// Define button pin for triggering config portal
#define BUTTON_PIN 0  // Change to your button pin if different (BOOT button on ESP32 is GPIO 0)

WiFiManager wm;

// Timing variables
unsigned long lastPrintTime = 0;
unsigned long lastCheckTime = 0;

void setup() {
    Serial.begin(115200);
    
    // Setup button pin
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    
    // Create WiFiManager object
    
    
    // Uncomment the next line to reset saved WiFi settings (for testing)
    // wm.resetSettings();
    
    // Check if button is pressed on startup to enter config portal
    
    
        // Normal auto-connect
    bool res = wm.autoConnect("ESP32-AP_Natawat", "password");
    
    if (!res) {
        Serial.println("Failed to connect to WiFi");
        Serial.println("Restarting ESP32...");
        ESP.restart();
    } else {
        Serial.println("Connected to WiFi successfully!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    }
}


void loop() {
    unsigned long currentMillis = millis();
    
    // Print IP address every 10 seconds
    if (currentMillis - lastPrintTime >= 10000) {
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        lastPrintTime = currentMillis;
    }
   
    // Check button every 100 ms
    if (currentMillis - lastCheckTime >= 100) {
        if (digitalRead(BUTTON_PIN) == LOW) {  // Button pressed (assuming LOW when pressed)
            Serial.println("Button pressed - Starting Config Portal");
            wm.startConfigPortal("ESP32-AP_Na", "password");
        }
        lastCheckTime = currentMillis;
    }
}