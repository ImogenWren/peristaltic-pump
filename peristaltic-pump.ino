/* Peristaltic Pump - Testing

    Peristaltic Pump using DRV8825 Stepper Motor Driver

    Pump Motor Specification:
    current per winding: 500 mA


    Pinout:

*/


#include <buttonObject.h>

#define BUTTON_PIN 2

buttonObject button(BUTTON_PIN, BUTTON_PULL_HIGH);


#define STEP_PIN 5
#define DIR_PIN 6
#define RESET_PIN 7
#define SLEEP_PIN 8
#define ENABLE_PIN 4

//DRV8824/8825

/*
|           |   |      |    |         |
|---        |---|---   |    |         | 
| ~{ENABLE} | 1 | ---- | 16 | VMOT    |
|         M0| 2 | ---- | 15 | GND     |
|         M1| 3 | ---- | 14 | B2      |
|         M2| 4 | ---- | 13 | B1      |
|   ~{RESET}| 5 | ---- | 14 | A1      |
|   ~{SLEEP}| 6 | ---- | 14 | A2      |
|       STEP| 7 | ---- | 14 | ~{FAULT}|
|        DIR| 8 | ---- | 14 | GND     |

*/

#define STEPS_PER_ML 10       // This value will need calibration
#define DISPENSED_PER_STEP 1  // Value found through experimentation

// Define initialal states
#define STEP_INIT false
#define DIR_INIT false
#define RESET_INIT true  // must be high?
#define SLEEP_INIT false
#define ENABLE_INIT false

// Run Mode
// RESET and SLEEP musst be pulled HIGH
// Enable can be left floating (default state is enabled ~{pull down})
//#define STEP_RUN false
//#define DIR_RUN false
#define RESET_RUN true
#define SLEEP_RUN true
#define ENABLE_RUN false

// Sleep Mode
#define RESET_SLEEP false
#define SLEEP_SLEEP false
#define ENABLE_SLEEP true


#define PULSES_PER_ML 5297
// #define PULSES_PER_ML 5297   // last tested value

void io_setup() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  pinMode(SLEEP_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
}

void set_direction(bool direction = true) {
  digitalWrite(DIR_PIN, direction);
}

/*
Timings here are important:
|           | min   | max   | delay |   |
|---        |---    |---    |---    |---|
|frq_step   |       | 250 Hz| 4 uS  |   |
|pulse_high | 1.9 uS|       | 2 uS  |   |
|pulse_low  | 1.9 uS|       | 2 uS  |   |
|enable_low | 650 nS|       | 1 uS  |   |
|sleep_high | 1.7 mS|       | 2 mS  |   |


*/





bool pump_active = false;

uint32_t startTime;
uint32_t finishTime;
uint32_t benchmarkTime;

void dispense_per_step(uint32_t no_steps = 1) {
  digitalWrite(ENABLE_PIN, ENABLE_RUN);
  digitalWrite(SLEEP_PIN, SLEEP_RUN);
  //  digitalWrite(RESET_PIN, RESET_RUN);

  delay(2);
  for (uint32_t i = 0; i < no_steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(4);
    digitalWrite(STEP_PIN, LOW);
    startTime = micros();
   // delayMicroseconds(4);
    delayMicroseconds(500);
  //  Serial.println(i);  // This takes ~ 492 microseconds
    finishTime = micros();
  }

  digitalWrite(ENABLE_PIN, ENABLE_SLEEP);
  digitalWrite(SLEEP_PIN, SLEEP_SLEEP);
  //  digitalWrite(RESET_PIN, RESET_SLEEP);
  delay(2);
  benchmarkTime = finishTime - startTime;
  Serial.println(benchmarkTime);
}

void dispense_large_vol(int16_t no_steps = 1) {
  digitalWrite(ENABLE_PIN, ENABLE_RUN);
  digitalWrite(SLEEP_PIN, SLEEP_RUN);
  //  digitalWrite(RESET_PIN, RESET_RUN);

  delay(2);
  for (int i = 0; i < no_steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(4);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(4);
    Serial.println(i);
  }
  digitalWrite(ENABLE_PIN, ENABLE_SLEEP);
  digitalWrite(SLEEP_PIN, SLEEP_SLEEP);
  //  digitalWrite(RESET_PIN, RESET_SLEEP);
  delay(2);
}


void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  Serial.println("Peristalitic Pump Setup");
  button.begin();
  io_setup();
  Serial.println("Outputs Set");
  set_direction(DIR_INIT);
  Serial.println("Direction set");
  digitalWrite(STEP_PIN, STEP_INIT);
  digitalWrite(RESET_PIN, RESET_INIT);  // Inital state
  digitalWrite(ENABLE_PIN, ENABLE_INIT);
  digitalWrite(SLEEP_PIN, SLEEP_INIT);
  Serial.println("Init State Set");
  delay(1000);
  pump_active = true;
  Serial.println("Pump Active");
}


void test_motor() {
}

uint16_t dispenseCount;
uint32_t totalDispensedCount;

// disp
void loop() {
  if (pump_active) {
    if (dispenseCount > 0) {
      //dispense_per_step(2645);  // this dispenses 0.5ml for some reason this pump needs 200 steps ish to move properly, and smallest step is 1/16th microstep
      //dispense_per_step(40000); // this dispenses 0.5ml for some reason this pump needs 200 steps ish to move properly, and smallest step is 1/16th microste
      
      dispense_per_step(PULSES_PER_ML); // dispense 1ml
      dispenseCount--;
      Serial.print("Fluid Dispensed, Count: ");
      Serial.print(dispenseCount);
      Serial.print(",  Total Fluid Dispensed, Count: ");
      Serial.println(totalDispensedCount);
    }
  }

  button.buttonLoop();
  if (button.shortPress) {
    button.buttonStats();
    dispenseCount = dispenseCount + 1;
    totalDispensedCount++;
    button.buttonReset();
    Serial.print("Dispense Added, Count: ");
    Serial.println(dispenseCount);
  }
}
