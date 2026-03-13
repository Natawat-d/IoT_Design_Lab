#include <WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// ============= HARDWARE CONFIGURATION =============
// DHT Sensor
#define DHTPIN 32
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// LEDs (Active LOW configuration)
#define LED1 2
#define LED2 4

// Buttons/Switches (Safe GPIO pins without boot conflicts)
#define SW1 18
#define SW2 27

// Config Portal Button
#define CONFIG_BUTTON 0  // BOOT button on ESP32

// ============= WIFI & MQTT CONFIGURATION =============
// WiFiManager will handle WiFi credentials

// MQTT Broker configuration (PLACEHOLDER - Update server IP)
const char* mqtt_server = "192.168.1.120";  // Replace with your MQTT broker IP
const int mqtt_port = 1883;
const char* mqtt_username = "ait@midterm";
const char* mqtt_password = "ait@1q2w3e4r";
const char* mqtt_client_id = "ESP32_125841";

// MQTT Topics
const char* publish_topic = "/esp32/data/125841";          // For sensor data
const char* status_topic = "/esp32/status/125841";            // For LED status changes
const char* subscribe_topic = "/esp32/control/#";  // For controlling LED1
const char* led1_topic = "/esp32/control/125841/led1";
const char* led2_topic = "/esp32/control/125841/led2";

// ============= GLOBAL OBJECTS =============
WiFiClient espClient;
PubSubClient client(espClient);
WiFiManager wm;

// ============= STATE VARIABLES =============
// LED states
bool led1State = false;
bool led2State = false;

// Button states for debouncing
bool lastSW1State = LOW;
bool lastSW2State = LOW;
unsigned long lastSW1PressTime = 0;
unsigned long lastSW2PressTime = 0;

// Timing variables
const unsigned long DEBOUNCE_DELAY = 200; // 200ms debounce (same as Lab3)
unsigned long lastSensorPublish = 0;
const unsigned long sensorPublishInterval = 30000; // 30 seconds
unsigned long lastConfigCheck = 0;
const unsigned long configCheckInterval = 100; // Check config button every 100ms

// ============= FUNCTION PROTOTYPES =============
void setupWiFi();
void reconnectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void publishSensorData();
void publishLEDStatus(int ledNumber, bool state);
void handleButtons();
void toggleLED1();
void toggleLED2();

// ============= SETUP =============
void setup() {
    Serial.begin(115200);
    Serial.println("\n=== ESP32 MQTT System Starting ===");
    
    // Initialize DHT sensor
    dht.begin();
    Serial.println("DHT sensor initialized");
    
    // Setup LED pins (Active LOW)
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    digitalWrite(LED1, HIGH); // OFF
    digitalWrite(LED2, HIGH); // OFF
    
    // Setup button pins with internal pullup
    pinMode(SW1, INPUT_PULLUP);
    pinMode(SW2, INPUT_PULLUP);
    pinMode(CONFIG_BUTTON, INPUT_PULLUP);
    
    // Connect to WiFi using WiFiManager
    setupWiFi();
    
    // Configure MQTT
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(mqttCallback);
    client.setBufferSize(512); // Increase buffer for JSON
    
    Serial.println("Setup complete!");
}

// ============= MAIN LOOP (Non-blocking) =============
void loop() {
    // Maintain MQTT connection
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop();
    
    // Handle button presses
    handleButtons();
    
    // Publish sensor data every 30 seconds
    unsigned long currentMillis = millis();
    if (currentMillis - lastSensorPublish >= sensorPublishInterval) {
        publishSensorData();
        lastSensorPublish = currentMillis;
    }
    
    // Check config button to open WiFi config portal
    if (currentMillis - lastConfigCheck >= configCheckInterval) {
        if (digitalRead(CONFIG_BUTTON) == LOW) {
            Serial.println("Config button pressed - Opening WiFi Config Portal");
            wm.startConfigPortal("ESP32_AP_125841", "ait12345");
        }
        lastConfigCheck = currentMillis;
    }
}

// ============= WIFI CONNECTION (using WiFiManager) =============
void setupWiFi() {
    Serial.println("Starting WiFiManager...");
    
    // Uncomment to reset WiFi settings for testing
    // wm.resetSettings();
    
    // Set timeout for config portal (3 minutes)
    wm.setConfigPortalTimeout(180);
    
    // Try to auto-connect to saved WiFi
    // If it fails, it starts an access point with name "ESP32_AP_125841" and password "ait12345"
    bool connected = wm.autoConnect("ESP32_AP_125841", "ait12345");
    
    if (!connected) {
        Serial.println("Failed to connect to WiFi - Restarting...");
        // Non-blocking wait using millis()
        unsigned long startWait = millis();
        while (millis() - startWait < 3000) {
            // Wait 3 seconds before restart
        }
        ESP.restart();
    }
    
    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

// ============= MQTT RECONNECTION =============
void reconnectMQTT() {
    // Try to reconnect only once per call (non-blocking)
    if (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        
        if (client.connect(mqtt_client_id, mqtt_username, mqtt_password)) {
            Serial.println("connected!");
            
            // Subscribe to control topic
            client.subscribe(subscribe_topic);
            Serial.print("Subscribed to: ");
            Serial.println(subscribe_topic);
            
            // Publish initial sensor data
            publishSensorData();
            
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" retrying in next loop...");
        }
    }
}

// ============= MQTT CALLBACK (Session 2: Subscribing) =============
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message received [");
    Serial.print(topic);
    Serial.print("]: ");
    
    // Convert payload to string
    String message = "";
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);
    
    // Parse JSON payload
  /* StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, message);
    
    if (error) {
        Serial.print("JSON parsing failed: ");
        Serial.println(error.c_str());
      
    }
    */
    // Control LED1 - accepts "on"/"off" strings
   // if (doc.containsKey("led1")) {
   if (String(topic) == led1_topic) {
   
        String led1Cmd = message;
        //String led1Cmd = doc["led1"].as<String>();
        if (led1Cmd == "on") {
            digitalWrite(LED1, LOW); // Active LOW
            led1State = true;
            Serial.println("LED1 turned ON (via MQTT)");
            publishLEDStatus(1, true);
        } else if (led1Cmd == "off") {
            digitalWrite(LED1, HIGH);
            led1State = false;
            Serial.println("LED1 turned OFF (via MQTT)");
            publishLEDStatus(1, false);
        }
    }
    
    // Control LED2 - accepts true/false boolean
    //if (doc.containsKey("led2")) {
    if (String(topic) == led2_topic) {
        bool led2Cmd = message == "true"; // payload for lde2 is expected to be "true" or "false"
        //bool led2Cmd = doc["led2"].as<bool>();
        if (led2Cmd) {
            digitalWrite(LED2, LOW); // Active LOW
            led2State = true;
            Serial.println("LED2 turned ON (via MQTT)");
            publishLEDStatus(2, true);
        } else if (!led2Cmd) {
            digitalWrite(LED2, HIGH);
            led2State = false;
            Serial.println("LED2 turned OFF (via MQTT)");
            publishLEDStatus(2, false);
        }
    }
}

// ============= PUBLISH SENSOR DATA (Session 1: Every 30s) =============
void publishSensorData() {
    // Read DHT sensor
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    
    // Check if readings are valid
    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }
    
    // Create JSON: {"data": {"sensor": {"temperature": xx, "humidity": xx}}}
    StaticJsonDocument<256> hardware;
    JsonObject doc = hardware.createNestedObject("hardware");
    JsonObject data = doc.createNestedObject("data");
    JsonObject sensor = data.createNestedObject("sensor");
    sensor["temp"] = round(temperature * 100) / 100.0; // 2 decimal places
    sensor["humi"] = round(humidity * 100) / 100.0;
    
    // Serialize and publish
    char jsonBuffer[256];
    serializeJson(hardware, jsonBuffer);
    
    if (client.publish(publish_topic, jsonBuffer)) {
        Serial.println("Sensor data published:");
        Serial.println(jsonBuffer);
    } else {
        Serial.println("Failed to publish sensor data");
    }
}

// ============= PUBLISH LED STATUS (Session 1: On state change) =============
void publishLEDStatus(int ledNumber, bool state) {
    StaticJsonDocument<256> doc;
    JsonObject data = doc.createNestedObject("data");
    JsonObject status = data.createNestedObject("status");
    
    // LED1 uses "on"/"off" strings, LED2 uses true/false
    if (ledNumber == 1) {
        status["led1"] = state ? "on" : "off";
    } else if (ledNumber == 2) {
        status["led2"] = state;
    }
    
    // Serialize and publish
    char jsonBuffer[256];
    serializeJson(doc, jsonBuffer);
    
    if (client.publish(status_topic, jsonBuffer)) {
        Serial.println("LED status published:");
        Serial.println(jsonBuffer);
    } else {
        Serial.println("Failed to publish LED status");
    }
}

// ============= BUTTON HANDLING (Session 1: Local Control) =============
void handleButtons() {
    unsigned long currentMillis = millis();
    
    // Read current button states
    bool sw1Current = digitalRead(SW1);
    bool sw2Current = digitalRead(SW2);
    
    // Handle Switch 1 (Toggle LED1) with debouncing - Lab3 pattern
    if (sw1Current != lastSW1State) 
    {
        Serial.print("SW1 state change detected: ");
        Serial.print(sw1Current);
        Serial.print(" -> lastState: ");
        Serial.println(lastSW1State);
        
        // Debounce check
        if (currentMillis - lastSW1PressTime >= DEBOUNCE_DELAY) 
        {
            lastSW1State = sw1Current;
            lastSW1PressTime = currentMillis;
            
            if (!sw1Current) {
                Serial.println("SW1 pressed - Toggling LED1");
                toggleLED1();
            }
        } else {
            Serial.println("SW1 debounce blocked");
        }
    }
    
    // Handle Switch 2 (Toggle LED2) with debouncing - Lab3 pattern
    if (sw2Current != lastSW2State) 
    {
        // Debounce check
        if (currentMillis - lastSW2PressTime >= DEBOUNCE_DELAY) 
        {
            lastSW2State = sw2Current;
            lastSW2PressTime = currentMillis;
            
            if (!sw2Current) {
                Serial.println("SW2 pressed - Toggling LED2");
                toggleLED2();
            }
        }
    }
}

// ============= TOGGLE LED1 =============
void toggleLED1() {
    led1State = !led1State;
    digitalWrite(LED1, led1State ? LOW : HIGH); // Active LOW
    Serial.print("LED1 toggled: ");
    Serial.println(led1State ? "ON" : "OFF");
    
    // Publish status change
    publishLEDStatus(1, led1State);
}

// ============= TOGGLE LED2 =============
void toggleLED2() {
    led2State = !led2State;
    digitalWrite(LED2, led2State ? LOW : HIGH); // Active LOW
    Serial.print("LED2 toggled: ");
    Serial.println(led2State ? "ON" : "OFF");
    
    // Publish status change
    publishLEDStatus(2, led2State);
}
