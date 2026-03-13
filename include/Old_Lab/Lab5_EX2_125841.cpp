#include <WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>

// Define LED pin
#define LED1 2  // Change to your LED pin if different

// Define button pin for triggering config portal
#define BUTTON_PIN 0  // BOOT button on ESP32 board

// NETPIE MQTT credentials (replace with your actual values from NETPIE dashboard)
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_client_id = "ea56ad81-03df-4977-a61d-cb81819d5ba9";  // Replace with your Client ID
const char* mqtt_username = "EP79qTUTG5sARi7PXnHjgASaSHiDGtPU";      // Replace with your Token
const char* mqtt_password = "ctmuifa2L9gCZZuApNzvwDyCp63CrRVQ";     // Replace with your Secret

// MQTT topic
const char* topic = "@msg/esp32/led1";

WiFiClient espClient;
PubSubClient client(espClient);
WiFiManager wm;

// Timing variable for button check
unsigned long lastCheckTime = 0;

void callback(char* topic, byte* payload, unsigned int length);
void reconnect();


void setup() {
    Serial.begin(115200);
    pinMode(LED1, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    digitalWrite(LED1, LOW);  // Start with LED off
    
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
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    
    // Check button every 100ms for config portal
    unsigned long currentMillis = millis();
    if (currentMillis - lastCheckTime >= 100) {
        if (digitalRead(BUTTON_PIN) == LOW) {  // Button pressed
            Serial.println("Button pressed - Starting Config Portal");
            wm.startConfigPortal("ESP32-AP_Natawat", "password");
            // After portal, MQTT should reconnect automatically
        }
        lastCheckTime = currentMillis;
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
    
    // Control LED based on message
    if (String(topic) == "@msg/esp32/led1") {
        if (message == "on") {
            digitalWrite(LED1, LOW);
            Serial.println("LED1 turned ON");
        } else if (message == "off") {
            digitalWrite(LED1, HIGH);
            Serial.println("LED1 turned OFF");
        }
    }
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(mqtt_client_id, mqtt_username, mqtt_password)) {
            Serial.println("connected");
            client.subscribe(topic);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}
