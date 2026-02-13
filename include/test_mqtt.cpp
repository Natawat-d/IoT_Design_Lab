#include <WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>

// NETPIE MQTT credentials
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_client_id = "ea56ad81-03df-4977-a61d-cb81819d5ba9";
const char* mqtt_username = "EP79qTUTG5sARi7PXnHjgASaSHiDGtPU";
const char* mqtt_password = "ctmuifa2L9gCZZuApNzvwDyCp63CrRVQ";

// MQTT topics
const char* test_topic = "@msg/esp32/test";
const char* subscribe_topic = "@msg/esp32/#";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);
}
WiFiManager wm;

// Counter variable
int counter = 0;

// Timing variable
unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 2000; // Publish every 2 seconds

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(mqtt_client_id, mqtt_username, mqtt_password)) {
            Serial.println("connected");
            client.subscribe(subscribe_topic);
            Serial.print("Subscribed to: ");
            Serial.println(subscribe_topic);
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
    
    // Publish increasing number every 2 seconds
    unsigned long currentMillis = millis();
    if (currentMillis - lastPublishTime >= publishInterval) {
        String message = String(counter);
        
        if (client.publish(test_topic, message.c_str())) {
            Serial.print("Published to @msg/esp32/test: ");
            Serial.println(counter);
            counter++;
        } else {
            Serial.println("Failed to publish");
        }
        
        lastPublishTime = currentMillis;
    }
}
