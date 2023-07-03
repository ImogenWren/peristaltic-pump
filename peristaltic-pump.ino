/* Peristaltic Pump - Testing

    Peristaltic Pump using DRV8825 Stepper Motor Driver

    Pump Motor Specification:
    current per winding: 500 mA


    Pinout:




*/


#define STEP_PIN 5
#define DIR_PIN 6
#define RESET_PIN 7
#define SLEEP_PIN 8
#define ENABLE_PIN 4

#define STEPS_PER_ML 10   // This value will need calibration

#define DISPENSED_PER_STEP  1    // Value found through experimentation

void output_setup() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  pinMode(SLEEP_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
}

void set_direction(bool direction = true) {
  digitalWrite(DIR_PIN, direction);
}



void do_pulse() {
  digitalWrite(SLEEP_PIN, HIGH);
  delay(100);
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(1000);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(10000);
  digitalWrite(SLEEP_PIN, LOW);
  delay(10);
}


bool pump_active = false;

void dispense_per_step(uint16_t no_steps = 1) {
  for (int i = 0; i < no_steps; i++) {
    do_pulse();
    Serial.print("Dispensing 1 Step. No. Steps Dispensed: ");
    Serial.println(i);
    delay(500);
  }
  Serial.println("Dispensing Finished");
  delay(2000);
 // pump_active = false;
}




void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  Serial.println("Peristalitic Pump Setup");
  output_setup();
  set_direction(true);
  digitalWrite(RESET_PIN, HIGH);   // Inital state
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(SLEEP_PIN, LOW);
  delay(1000);
  pump_active = true;
}


void test_motor(){

}


void loop() {
  if (pump_active) {
    dispense_per_step(10);
  } 
}
