#include <WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Define DHT sensor
#define DHTPIN 32        // DHT sensor pin
#define DHTTYPE DHT22    // DHT22 or DHT11
DHT dht(DHTPIN, DHTTYPE);

// Define LED pins
#define LED1 2   // GPIO 2
#define LED2 4   // GPIO 4
#define SW1 15   // GPIO 15
#define SW2 17   // GPIO 17

// Define button pin for triggering config portal
#define BUTTON_PIN 0  // BOOT button on ESP32 board

// NETPIE MQTT credentials
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_client_id = "ea56ad81-03df-4977-a61d-cb81819d5ba9";
const char* mqtt_username = "EP79qTUTG5sARi7PXnHjgASaSHiDGtPU";
const char* mqtt_password = "ctmuifa2L9gCZZuApNzvwDyCp63CrRVQ";

// MQTT topics
const char* topic = "@msg/#";
const char* shadow_topic = "@shadow/data/update";

WiFiClient espClient;
PubSubClient client(espClient);
WiFiManager wm;

// Timing variables
unsigned long lastCheckTime = 0;
unsigned long lastSensorRead = 0;
const unsigned long sensorInterval = 5000; // Read sensor every 5 seconds

// LED state tracking
bool led1State = false;
bool led2State = false;
bool led3State = false;

// Previous LED states to detect changes
bool prev_led1State = false;
bool prev_led2State = false;
bool prev_led3State = false;

void publishShadow(bool forceLED = false) {
    // Read DHT sensor
    float humidity = dht.readHumidity();
    float tempC = dht.readTemperature();
    
    // Check if reading failed
    if (isnan(humidity) || isnan(tempC)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }
    
    // Create JSON document
    StaticJsonDocument<256> doc;
    JsonObject data = doc.createNestedObject("data");
    
    // Add temperature (Celsius) and humidity (always sent)
    data["temperature"] = round(tempC * 100) / 100.0; // Round to 2 decimal places
    data["humidity"] = round(humidity * 100) / 100.0;
    
    // Add LED status only if changed or forced
    if (forceLED || led1State != prev_led1State) {
        data["led1"] = led1State;
        prev_led1State = led1State;
    }
    if (forceLED || led2State != prev_led2State) {
        data["led2"] = led2State;
        prev_led2State = led2State;
    }
    if (forceLED || led3State != prev_led3State) {
        data["led3"] = led3State;
        prev_led3State = led3State;
    }
    
    // Serialize JSON to string
    char jsonBuffer[256];
    serializeJson(doc, jsonBuffer);
    
    // Publish to shadow (for dashboard display)
    if (client.publish(shadow_topic, jsonBuffer)) {
        Serial.println("Published to Device Shadow:");
        Serial.println(jsonBuffer);
    } else {
        Serial.println("Failed to publish to Device Shadow");
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);
    
    bool stateChanged = false;
    
    // Control LEDs from JSON message on @msg/esp32/led topic
    if (String(topic) == "@msg/esp32/led") {
        // Parse JSON
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, message);
        
        if (error) {
            Serial.print("JSON parsing failed: ");
            Serial.println(error.c_str());
            return;
        }
        
        // Control LED1
        if (doc.containsKey("led1")) {
            String led1Cmd = doc["led1"].as<String>();
            if (led1Cmd == "on") {
                digitalWrite(LED1, LOW);
                led1State = true;
                Serial.println("LED1 turned ON");
                stateChanged = true;
            } else if (led1Cmd == "off") {
                digitalWrite(LED1, HIGH);
                led1State = false;
                Serial.println("LED1 turned OFF");
                stateChanged = true;
            }
        }
        
        // Control LED2
        if (doc.containsKey("led2")) {
            String led2Cmd = doc["led2"].as<String>();
            if (led2Cmd == "on") {
                digitalWrite(LED2, LOW);
                led2State = true;
                Serial.println("LED2 turned ON");
                stateChanged = true;
            } else if (led2Cmd == "off") {
                digitalWrite(LED2, HIGH);
                led2State = false;
                Serial.println("LED2 turned OFF");
                stateChanged = true;
            }
        }
        
        // Control LED3
        if (doc.containsKey("led3")) {
            String led3Cmd = doc["led3"].as<String>();
            if (led3Cmd == "on") {
                digitalWrite(LED3, LOW);
                led3State = true;
                Serial.println("LED3 turned ON");
                stateChanged = true;
            } else if (led3Cmd == "off") {
                digitalWrite(LED3, HIGH);
                led3State = false;
                Serial.println("LED3 turned OFF");
                stateChanged = true;
            }
        }
    }
    
    // Publish shadow immediately when LED state changes
    if (stateChanged) {
        publishShadow(true); // Force publish all LED states
    }
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(mqtt_client_id, mqtt_username, mqtt_password)) {
            Serial.println("connected");
            // Subscribe to LED control topic
            client.subscribe(topic);
            Serial.println("Subscribed to @msg/esp32/led (JSON LED control)");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
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
    
    // MQTT setup
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    client.setBufferSize(512); // Increase buffer for JSON
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    
    // Read sensor and publish to shadow periodically (for dashboard)
    unsigned long currentMillis = millis();
    if (currentMillis - lastSensorRead >= sensorInterval) {
        publishShadow(false); // Only publish changed LED states
        lastSensorRead = currentMillis;
    }
    
    // Check button every 100ms for config portal
    if (currentMillis - lastCheckTime >= 100) {
        if (digitalRead(BUTTON_PIN) == LOW) {  // Button pressed
            Serial.println("Button pressed - Starting Config Portal");
            wm.startConfigPortal("ESP32-AP_Natawat", "password");
        }
        lastCheckTime = currentMillis;
    }
}
