#include <Arduino.h>

#define LED1 2              // LED1 connected to GPIO 2 (built-in, active LOW)
#define LED2 4              // LED2 connected to GPIO 4
#define LED3 5              // LED3 connected to GPIO 5
#define SW2 18              // Switch 2 connected to GPIO 12
#define SW3 19              // Switch 3 connected to GPIO 13
#define POT_PIN 35          // Potentiometer connected to GPIO 35 (ADC)


bool lastSW2State = LOW;
bool lastSW3State = LOW;
bool sw2State = false;      // false = LED OFF, true = LED ON
int sw3State = 0;           // 0-6 for 7 states
unsigned long lastSW2PressTime = 0;
unsigned long lastSW3PressTime = 0;
const unsigned long DEBOUNCE_DELAY = 200; // 200ms debounce

// Timing for potentiometer reading
unsigned long previousPotMillis = 0;
const unsigned long POT_INTERVAL = 2000; // Read pot every 2 seconds


//function prototype
int sw3_judgement(int sw3State);

void setup() 
{
  // Initialize serial communication
  Serial.begin(115200);
  
  // Initialize LED pins as output
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  
  // Initialize switch pins as input with pull-up
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
  
  // Initialize all LEDs to OFF state
  digitalWrite(LED1, HIGH);  
  digitalWrite(LED2, HIGH);   
  digitalWrite(LED3, HIGH);   
  
  Serial.println("System ready. Press SW2 or SW3 to control LEDs.");
}

void loop() 
{
  unsigned long currentMillis = millis();
  
  // Read switch states (LOW = pressed with pull-up)
  bool sw2Current = digitalRead(SW2);
  bool sw3Current = digitalRead(SW3); 
  /*SW2 Check - Toggle LED1*/
  if (sw2Current != lastSW2State ) 
  {
    // Debounce check
    if (currentMillis - lastSW2PressTime >= DEBOUNCE_DELAY) 
    {
      
      lastSW2State = sw2Current;
      lastSW2PressTime = currentMillis;
      
      if (!sw2Current){
        // Toggle LED1 state
        sw2State = !sw2State;
        if (sw2State) 
        {
          digitalWrite(LED1, LOW);  // Turn LED1 ON (active LOW)
          Serial.println("SW2 Press: LED1 ON");
        } 
        else 
        {
          digitalWrite(LED1, HIGH); // Turn LED1 OFF
          Serial.println("SW2 Press: LED1 OFF");
        }
      
      }
    }
  }
  
  /*SW3 Check - Toggle pressing*/
  if (sw3Current != lastSW3State) 
  {
    // Debounce check
    if (currentMillis - lastSW3PressTime >= DEBOUNCE_DELAY) 
    {
      
      lastSW3State = sw3Current;
      lastSW3PressTime = currentMillis;
      
      if (!sw3Current) {
        Serial.println("SW3 Press: State "+String(sw3State));
        sw3_judgement(sw3State);
        sw3State = (sw3State + 1) % 7;
        

      }
    }
  }

  
  if (currentMillis - previousPotMillis >= POT_INTERVAL) {
    previousPotMillis = currentMillis;
    
    int potValue = analogRead(POT_PIN);
    Serial.print("Potentiometer Value: ");
    Serial.print(potValue);
    Serial.print(" (");
    Serial.print((potValue * 3.3) / 4095.0, 2);
    Serial.println(" V)");
  }
}

__inline__ int sw3_judgement(int sw3State)
{
    switch (sw3State) {
            case 0: // 1st press: LED2 ON, LED3 OFF
            digitalWrite(LED2, LOW);
            digitalWrite(LED3, HIGH);
            Serial.println("SW3 Press 1: LED2 ON, LED3 OFF");
            break;
            case 1: // 2nd press: LED2 OFF, LED3 OFF
            digitalWrite(LED2, HIGH);
            digitalWrite(LED3, HIGH);
            Serial.println("SW3 Press 2: LED2 OFF, LED3 OFF");
            break;
            case 2: // 3rd press: LED2 OFF, LED3 ON
            digitalWrite(LED2, HIGH);
            digitalWrite(LED3, LOW);
            Serial.println("SW3 Press 3: LED2 OFF, LED3 ON");
            break;
            case 3: // 4th press: LED2 OFF, LED3 OFF
            digitalWrite(LED2, HIGH);
            digitalWrite(LED3, HIGH);
            Serial.println("SW3 Press 4: LED2 OFF, LED3 OFF");
            break;
            case 4: // 5th press: LED2 ON, LED3 ON
            digitalWrite(LED2, LOW);
            digitalWrite(LED3, LOW);
            Serial.println("SW3 Press 5: LED2 ON, LED3 ON");
            break;
            case 5: // 6th press: LED2 OFF, LED3 OFF
            digitalWrite(LED2, HIGH);
            digitalWrite(LED3, HIGH);
            Serial.println("SW3 Press 6: LED2 OFF, LED3 OFF");
            break;
            case 6: // 7th press: LED2 ON, LED3 OFF (cycle back)
            digitalWrite(LED2, LOW);
            digitalWrite(LED3, HIGH);
            Serial.println("SW3 Press 7: LED2 ON, LED3 OFF (cycle restarts)");
            break;
        }
        return 0;

}