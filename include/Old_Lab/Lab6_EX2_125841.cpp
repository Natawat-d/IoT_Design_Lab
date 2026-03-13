/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID           "TMPL6AcdKLm5q"
#define BLYNK_TEMPLATE_NAME         "Quickstart Template"
#define BLYNK_AUTH_TOKEN            "oBB7Nqq5299b8V-FccOmyIZJZX8cmXX6"

#include <WiFi.h>
#include <WiFiManager.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Define DHT sensor
#define DHTPIN 32        // DHT sensor pin
#define DHTTYPE DHT22    // DHT22 or DHT11
DHT dht(DHTPIN, DHTTYPE);

// Define LED pins
#define LED1 2   // GPIO 2
#define LED2 4   // GPIO 4
#define LED3 15  // GPIO 15

// Define button pin for triggering config portal
#define BUTTON_PIN 0  // BOOT button on ESP32 board

// Blynk Auth Token
char auth[] = BLYNK_AUTH_TOKEN;

// Virtual Pins for Blynk Dashboard
// V0 - Temperature (Label, Gauge, Chart)
// V1 - Humidity (Label, Gauge, Chart)
// V2 - LED1 Switch
// V3 - LED2 Switch
// V4 - LED3 Switch
// V5 - LED1 Status LED
// V6 - LED2 Status LED
// V7 - LED3 Status LED

WiFiManager wm;
BlynkTimer timer;

// LED state tracking
bool led1State = false;
bool led2State = false;
bool led3State = false;

// Timing variables
unsigned long lastCheckTime = 0;

// Function to send sensor data to Blynk
void sendSensorData() {
    float humidity = dht.readHumidity();
    float tempC = dht.readTemperature();
    
    // Check if reading failed
    if (isnan(humidity) || isnan(tempC)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }
    
    // Send to Blynk virtual pins
    Blynk.virtualWrite(V0, tempC);      // Temperature
    Blynk.virtualWrite(V1, humidity);   // Humidity
    
    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
}

// Blynk Virtual Pin Handlers for LED Control

// LED1 Control (V2)
BLYNK_WRITE(V2) {
    int value = param.asInt();
    if (value == 1) {
        digitalWrite(LED1, LOW);  // Turn ON (active LOW)
        led1State = true;
        Blynk.virtualWrite(V5, 255);  // Update LED1 status widget
        Serial.println("LED1 turned ON");
    } else {
        digitalWrite(LED1, HIGH);  // Turn OFF
        led1State = false;
        Blynk.virtualWrite(V5, 0);
        Serial.println("LED1 turned OFF");
    }
}

// LED2 Control (V3)
BLYNK_WRITE(V3) {
    int value = param.asInt();
    if (value == 1) {
        digitalWrite(LED2, LOW);  // Turn ON (active LOW)
        led2State = true;
        Blynk.virtualWrite(V6, 255);  // Update LED2 status widget
        Serial.println("LED2 turned ON");
    } else {
        digitalWrite(LED2, HIGH);  // Turn OFF
        led2State = false;
        Blynk.virtualWrite(V6, 0);
        Serial.println("LED2 turned OFF");
    }
}

// LED3 Control (V4)
BLYNK_WRITE(V4) {
    int value = param.asInt();
    if (value == 1) {
        digitalWrite(LED3, LOW);  // Turn ON (active LOW)
        led3State = true;
        Blynk.virtualWrite(V7, 255);  // Update LED3 status widget
        Serial.println("LED3 turned ON");
    } else {
        digitalWrite(LED3, HIGH);  // Turn OFF
        led3State = false;
        Blynk.virtualWrite(V7, 0);
        Serial.println("LED3 turned OFF");
    }
}

// Sync LED status on connection
BLYNK_CONNECTED() {
    Serial.println("Blynk Connected!");
    // Sync LED states with Blynk app
    Blynk.virtualWrite(V2, led1State ? 1 : 0);
    Blynk.virtualWrite(V3, led2State ? 1 : 0);
    Blynk.virtualWrite(V4, led3State ? 1 : 0);
    Blynk.virtualWrite(V5, led1State ? 255 : 0);
    Blynk.virtualWrite(V6, led2State ? 255 : 0);
    Blynk.virtualWrite(V7, led3State ? 255 : 0);
}

void setup() {
    Serial.begin(115200);
    
    // Initialize DHT sensor
    dht.begin();
    
    // Setup LED pins
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    
    // Start with all LEDs off (HIGH for active LOW LEDs)
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    
    // WiFiManager setup
    Serial.println("Starting WiFiManager...");
    // wm.resetSettings();  // Uncomment to reset WiFi settings
    
    bool res = wm.autoConnect("ESP32-AP_Natawat", "password");
    
    if (!res) {
        Serial.println("Failed to connect to WiFi");
        ESP.restart();
    } else {
        Serial.println("Connected to WiFi");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    }
    
    // Blynk setup
    Blynk.config(auth);
    Blynk.connect();
    
    // Setup timer to send sensor data every 5 seconds
    timer.setInterval(5000L, sendSensorData);
    
    Serial.println("Setup completed. Blynk Dashboard ready!");
}

void loop() {
    Blynk.run();
    timer.run();
    
    // Check button for config portal
    unsigned long currentMillis = millis();
    if (currentMillis - lastCheckTime >= 100) {
        if (digitalRead(BUTTON_PIN) == LOW) {  // Button pressed
            Serial.println("Button pressed - Starting Config Portal");
            wm.startConfigPortal("ESP32-AP_Natawat", "password");
        }
        lastCheckTime = currentMillis;
    }
}
