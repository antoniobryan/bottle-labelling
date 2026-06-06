#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <KY040.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int STEPPER1_DIR  = 5;
const int STEPPER1_STEP = 2;
const int STEPPER2_DIR  = 6;
const int STEPPER2_STEP = 3;

const int smallest_value = 10;
const int largest_value  = 500;

// 3.75 degree motors are used, hence values under 1500 for stepdelay will make them stall.

#define STEPS_PER_MM  3
#define STEP_DELAY_US 4000

volatile bool switch_NOT_pressed = true;
volatile int  v_value     = 100;
int           previous_value = 9999;

#define CLK_PIN        9
#define DT_PIN         10
#define encoder_switch 11

KY040 g_rotaryEncoder(CLK_PIN, DT_PIN);

ISR(PCINT0_vect) {
  if (!digitalRead(encoder_switch)) {
    switch_NOT_pressed = false;
  }
  switch (g_rotaryEncoder.getRotation()) {
    case KY040::CLOCKWISE:
      v_value += 5;
      if (v_value > largest_value) v_value = largest_value;
      break;
    case KY040::COUNTERCLOCKWISE:
      v_value -= 5;
      if (v_value < smallest_value) v_value = smallest_value;
      break;
  }
}

void print_value() {
  if (previous_value != v_value) {
    lcd.setCursor(7, 1);
    lcd.print("     ");
    lcd.setCursor(7, 1);
    lcd.print(v_value);
    lcd.print("mm");
    Serial.print("[LCD] Length: ");
    Serial.print(v_value);
    Serial.println(" mm");
    previous_value = v_value;
  }
}

void show_home_screen() {
  switch_NOT_pressed = true;
  previous_value = 9999;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Team A17");
  lcd.setCursor(0, 1);
  lcd.print("Length:");
  print_value();
  Serial.println("[LCD Row0]    Team A17");
  Serial.println("[LCD Row1] Length: XXmm");
}

void dispense() {
  long total_steps = (long)v_value * STEPS_PER_MM;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dispensing ");
  lcd.print(v_value);
  lcd.print("mm");
  Serial.print("[LCD] Dispensing ");
  Serial.print(v_value);
  Serial.print("mm = ");
  Serial.print(total_steps);
  Serial.println(" steps");

  delay(1000);  // 1-second pause so the screen is readable before motors start

  digitalWrite(STEPPER1_DIR, HIGH);
  digitalWrite(STEPPER2_DIR, HIGH);

  for (long i = 0; i < total_steps; i++) {
    digitalWrite(STEPPER1_STEP, HIGH);
    digitalWrite(STEPPER2_STEP, HIGH);
    delayMicroseconds(STEP_DELAY_US);
    digitalWrite(STEPPER1_STEP, LOW);
    digitalWrite(STEPPER2_STEP, LOW);
    delayMicroseconds(STEP_DELAY_US);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Done!");
  lcd.setCursor(0, 1);
  lcd.print(v_value);
  lcd.print("mm dispensed");
  Serial.println("[LCD] Dispensed!");
  delay(1500);
}

void setup() {
  Serial.begin(115200);
  pinMode(STEPPER1_DIR,  OUTPUT);
  pinMode(STEPPER1_STEP, OUTPUT);
  pinMode(STEPPER2_DIR,  OUTPUT);
  pinMode(STEPPER2_STEP, OUTPUT);
  pinMode(CLK_PIN,        INPUT_PULLUP);
  pinMode(DT_PIN,         INPUT_PULLUP);
  pinMode(encoder_switch, INPUT_PULLUP);
  PCICR  |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT1) | (1 << PCINT2) | (1 << PCINT3);
  lcd.init();
  lcd.backlight();
  show_home_screen();
  while (switch_NOT_pressed) {
    print_value();
  }
  dispense();
}

void loop() {
  show_home_screen();
  while (switch_NOT_pressed) {
    print_value();
  }
  dispense();
}