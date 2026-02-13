/*Compose By
Natawat Damrongsilp
st125841
*/

#include <Arduino.h>


//Change LAB_TASK to 3 for EX3
//Change LAB_TASK to 4 for EX4
#define LAB_TASK 4


// Define LED pins
#define LED1 2
#define LED2 4
#define LED3 5

void setup() {
  // Set LED pins as outputs
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

void step1_3();
void step2_3();
void step3_3();
void step4_3();
void step1_4();
void step2_4();
void step3_4();
void step4_4();

void loop() {

    #if LAB_TASK == 3
        step1_3();   
        step2_3();
        step3_3();
        step4_3();
    #elif LAB_TASK == 4
        step1_4();
        step2_4();
        step3_4();
        step4_4();
    #endif



  // Loop back to step 1
}


/***************************************************************/
/*START EX3*/
void step1_3() {
  // All LEDs blink 1 time (active low: LOW = on, HIGH = off)
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  delay(500);
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  delay(500);
}

void step2_3() {
  // Turn on in sequence LED1, LED2, LED3 (LOW = on)
  digitalWrite(LED1, LOW);
  delay(500);
  digitalWrite(LED2, LOW);
  delay(500);
  digitalWrite(LED3, LOW);
  delay(500);
}

void step3_3() {
  // Turn off in sequence LED3, LED2, LED1 (HIGH = off)
  digitalWrite(LED3, HIGH);
  delay(500);
  digitalWrite(LED2, HIGH);
  delay(500);
  digitalWrite(LED1, HIGH);
  delay(500);
}

void step4_3() {
  // All LEDs blink 2 times (active low: LOW = on, HIGH = off)
  for (int i = 0; i < 2; i++) {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    delay(500);
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    delay(500);
  }
}
/*END EX3*/
/******************************************************************** */



/*START EX4*/

void step1_4() {
   digitalWrite(LED1, HIGH);
   digitalWrite(LED2, HIGH);
   digitalWrite(LED3, HIGH);
  // LED1 blink 3 times (active low: LOW = on, HIGH = off)
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED1, LOW);
    delay(500);
    digitalWrite(LED1, HIGH);
    delay(500);
  }
}

void step2_4() {

  // LED2 blink 5 times
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED2, LOW);
    delay(500);
    digitalWrite(LED2, HIGH);
    delay(500);
  }
}

void step3_4() {
  // LED3 blink 2 times
  for (int i = 0; i < 2; i++) {
    digitalWrite(LED3, LOW);
    delay(500);
    digitalWrite(LED3, HIGH);
    delay(500);
  }
}

void step4_4() {
  // LED1, LED2, LED3 blink together 3 times
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    delay(500);
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    delay(500);
  }
}
/*END EX4*/