// TIMER
#include <SimpleTimer.h>
SimpleTimer Timer1(20000);
SimpleTimer Timer2(20000);
SimpleTimer speedTimer1(2000);
SimpleTimer speedTimer2(5000);


// STEPPERS
const int enPin = 8;
const int stepPin1 = 2;
const int dirPin1 = 5;

int stepDelay1 = 200;

void setup() {
  Serial.begin(9600);
  // STEPPERS
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW);
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  digitalWrite(dirPin1, HIGH);
  // LIMITS
  Serial.begin(9600);


}



void loop() {
  stepA();
  aleaSpeed1();
}



void stepA() {
  digitalWrite(stepPin1, HIGH);
  delayMicroseconds(stepDelay1);
  digitalWrite(stepPin1, LOW);
}



void aleaSpeed1() {
  int s1 = random(400, 1500);
  if (speedTimer1.isReady()) {
    stepDelay1 = s1;
    speedTimer1.reset();
  }
}
