// TIMER
#include <SimpleTimer.h>
#include <ezButton.h> //Librairie pour endstops
#include <Servo.h>

Servo servo1;
Servo servo2;

SimpleTimer servoTimer1(5000);
SimpleTimer servoTimer2(2000);
SimpleTimer speedTimer1(2000);
SimpleTimer speedTimer2(5000);


//Déclaration endstops
ezButton limitSwitch1(43); 
ezButton limitSwitch2(39);
ezButton limitSwitch3(35);
ezButton limitSwitch4(31);

// STEPPERS
#define enPin 8
#define stepPin1 2
#define dirPin1 5
#define stepPin2 3
#define dirPin2 6

int stepDelay1 = 200;
int stepDelay2 = 200;

bool servoPosition1 = false;
bool servoPosition2 = false;

int servoPosition[2] = {10, 30};


void setup() {
  Serial.begin(9600);

  //Debounce pour endstops
  limitSwitch1.setDebounceTime(50);
  limitSwitch2.setDebounceTime(50);
  limitSwitch3.setDebounceTime(50);
  limitSwitch4.setDebounceTime(50);
  
  // STEPPERS
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW);
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, HIGH);

  servo1.attach(23);
  servo2.attach(25);

}


void loop() {
  
  //Loop des enstops
  limitSwitch1.loop();
  limitSwitch2.loop();
  limitSwitch3.loop();
  limitSwitch4.loop();

  goStepMotor(stepDelay1, stepPin1);
  goStepMotor(stepDelay2, stepPin2);

   //Variables permetant le changement de sens des steppers en fonction des limites activées
    if (limitSwitch1.isPressed()) {
      digitalWrite(dirPin1, 1);
      Serial.println("The button 1 is pressed");
      moveAway(stepDelay1, stepPin1);
    }
    if (limitSwitch2.isPressed()) {
      digitalWrite(dirPin1, 0);
      Serial.println("The button 2 is pressed");
      moveAway(stepDelay1, stepPin1);
    }
    if (limitSwitch3.isPressed()) {
      digitalWrite(dirPin2, 0);
      Serial.println("The button 3 is pressed");
      moveAway(stepDelay2, stepPin2);
    }
    if (limitSwitch4.isPressed()) {
      digitalWrite(dirPin2, 1);
      Serial.println("The button 4 is pressed");
      moveAway(stepDelay2, stepPin2);
    }
  
  //stepA();
  aleaSpeed1();
  aleaSpeed2();
  aleaServo1();
  aleaServo2();

}
 


// Function to move a stepper motor one step
void goStepMotor(int stepDelay, int stepPin) {
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(stepDelay);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(stepDelay);
}

//Fonction d'éloignement lors du contact avec un endstop
void moveAway(int stepDelay, int stepPin) {
  for (int x = 0; x < 200; x++) {
    goStepMotor(stepDelay, stepPin);
  }
}

//
//void stepA() {
//  digitalWrite(stepPin1, HIGH);
//  delayMicroseconds(stepDelay1);
//  digitalWrite(stepPin1, LOW);
//}

void aleaSpeed1() {
  int s1 = random(300, 800);
  if (speedTimer1.isReady()) {
    stepDelay1 = s1;
    speedTimer1.reset();
  }
}

void aleaSpeed2() {
  int s2 = random(300, 800);
  if (speedTimer2.isReady()) {
    stepDelay2 = s2;
    speedTimer2.reset();
  }
}

void aleaServo2(){
  int s2 = random(2);
  if (servoTimer2.isReady()) {
      servo2.write(servoPosition[s2]);
      servoTimer2.reset();
      Serial.println(servoPosition[s2]);

}
}

void aleaServo1(){
  int s1 = random(2);
  if (servoTimer1.isReady()) {
      servo1.write(servoPosition[s1]);
      servoTimer1.reset();

}
}
