#include <ezButton.h> //Librairie pour endstops
#include <SimpleTimer.h> //Timer pour les variables aléatoires

//Déclaration endstops
ezButton limitSwitch1(42);
ezButton limitSwitch2(43);
ezButton limitSwitch3(41);
ezButton limitSwitch4(40);

//Déclaration steppers
#define stepPin1 2
#define dirPin1 5
#define stepPin2 3
#define dirPin2 6
#define stepPin3 4
#define dirPin3 7

//Déclarations timers pour variables aléatoires
SimpleTimer timeTimer(8000);
SimpleTimer speedTimer(8000);
SimpleTimer spinTimer(5000);

//Vitesse de base des steppers
int stepDelay = 200;

//Variables to store the maximum of steps between the 0 and max position for each direction
int stepNumberMotorHorizontal = 0;
int stepNumberMotorVertical = 0;

// Input string
String inString="";
char string[128];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Debounce pour endstops
  limitSwitch1.setDebounceTime(50);
  limitSwitch2.setDebounceTime(50);
  limitSwitch3.setDebounceTime(50);
  limitSwitch4.setDebounceTime(50);
  //Setup des steppers
  pinMode(dirPin1, OUTPUT);
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin3, OUTPUT);
  pinMode(stepPin3, OUTPUT);
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, HIGH);
  digitalWrite(dirPin3, HIGH);

  // vertical calibration
  setMotorToZero(true);
  stepNumberMotorVertical = motorCalibration(true);

  Serial.print("Number of steps vertical :");
  Serial.print(stepNumberMotorVertical);
  Serial.println();

  // horizontal calibration
  setMotorToZero(false);
  stepNumberMotorHorizontal = motorCalibration(false);

  Serial.print("Number of steps horizontal :");
  Serial.print(stepNumberMotorHorizontal);
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:

  //Loop des enstops
  limitSwitch1.loop();
  limitSwitch2.loop();
  limitSwitch3.loop();
  limitSwitch4.loop(); 

//  if(Serial.available()){
//    String receivedString = Serial.readString();
//   receivedString.trim();                        // remove any \r \n whitespace at the end of the String
//    receivedString.toCharArray(string, sizeof(string));
//    i = 0;
//   & p = strtok(string, "-");
//    while(p && i < 3)
//    {
//      words[i] = p;
//     p = strtok(NULL, delimiter);
//     ++i;
//   }
//
//   for(i = 0; i < 3; ++i)
//   {
//     Serial.println(words[i]);
//   }
 // }

  goMotor(stepPin1);
  goMotor(stepPin2);
  goMotor(stepPin3);

    //Variables permetant le changement de sens des steppers en fonction des limites activées
  if (limitSwitch1.isPressed()) {
     digitalWrite(dirPin1, 0);
     digitalWrite(dirPin2, 1);
     Serial.println("The button 1 is pressed");
     moveAway();
  }
  if (limitSwitch2.isPressed()) {
     digitalWrite(dirPin1, 1);
     digitalWrite(dirPin2, 0);
     Serial.println("The button 2 is pressed");
     moveAway();
  }
  if (limitSwitch3.isPressed()) {
    digitalWrite(dirPin1, 0);
    digitalWrite(dirPin2, 0);
    Serial.println("The button 3 is pressed");
    moveAway();
  }
  if (limitSwitch4.isPressed()) {
    digitalWrite(dirPin1, 1);
    digitalWrite(dirPin2, 1);
    Serial.println("The button 4 is pressed");
    moveAway();
   }

  // Appel des fonctions aléatoires
  aleaTime();
  aleaSpeed();
  aleaSpin();

}

void goMotor(int stepPin){
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(stepDelay);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(stepDelay);
}

//Fonction d'éloignement lors du contact avec un endstop
void moveAway() {
  for (int x = 0; x < 500; x++) {
    goMotor(stepPin1);
    goMotor(stepPin2);
  }
}

// Fonction d'aléatoire pour le déplacement (sens rotation steppers) à partir d'un timer
// Si le timer est prêt (temps défini plus haut) et que le tirage au sort en modulo 2 
// est validé, alors chacun des stepper sur XY obtient un aléatoire pour son sens de rotation 
void aleaTime() {
  int r = random(0, 3);
  r = r % 2;
  if (timeTimer.isReady() && r > 0) {
    digitalWrite(dirPin1, random(0, 2));
    digitalWrite(dirPin2, random(0, 2));
    timeTimer.reset();
  }
}

//Fonction d'aléatoire de la vitesse de déplacement 
//Même principe que pour le déplacement mais avec le stepdelay (temps entre deux steps)
//et avec un timer différent
void aleaSpeed() {
  int s = random(500, 1000);
  if (speedTimer.isReady()) {
    stepDelay = s;
    speedTimer.reset();
  }
}

void setSpeed(int s) {
  if (speedTimer.isReady()) {
    stepDelay = s;
    speedTimer.reset();
  }
}

void setDir(int dir1, int dir2){
  if (timeTimer.isReady()) {
    digitalWrite(dirPin1, dir2);
    digitalWrite(dirPin2, dir2);
    timeTimer.reset();
  }
}

void setSpin(int d){
  d = d % 2;
  if (spinTimer.isReady() && d > 0) {
    digitalWrite(dirPin3, random(0, 2));
    spinTimer.reset();
  }
}

//Fonction d'aléatoire pour le sens de rotation de la base magnétique
//Idem au déplacement XY et avec un timer différent encore
void aleaSpin() {
  int d = random(0, 3);
  d = d % 2;
  if (spinTimer.isReady() && d > 0) {
    digitalWrite(dirPin3, random(0, 2));
    spinTimer.reset();
  }
}

void setMotorToZero(bool vertical){
  if(vertical){
    digitalWrite(dirPin1, HIGH);
    digitalWrite(dirPin2, HIGH);
    
  }else{
    digitalWrite(dirPin1, HIGH);
    digitalWrite(dirPin2, LOW);
  }
  
  bool motorCalibrationEnd = false;
  
  while(motorCalibrationEnd == false){
    //Loop des enstops
    limitSwitch1.loop();
    limitSwitch2.loop();
    limitSwitch3.loop();
    limitSwitch4.loop();
    
    // 1 step per motor
    goMotor(stepPin1);
    goMotor(stepPin2);

    if(vertical){
      if (limitSwitch3.isPressed()) {
            digitalWrite(dirPin1, 0);
            digitalWrite(dirPin2, 0); 
            Serial.println("The button is pressed, 0 is set");
            moveAway();
            motorCalibrationEnd = true;
      }
     }else{ 
       if (limitSwitch1.isPressed()) {
            digitalWrite(dirPin1, 0);
            digitalWrite(dirPin2, 1); 
            Serial.println("The button is pressed, 0 is set");
            moveAway();
            motorCalibrationEnd = true;
        }
      }
    }
}

int motorCalibration(bool vertical){
   if(vertical){
    digitalWrite(dirPin1, LOW);
    digitalWrite(dirPin2, LOW);
    
  }else{
    digitalWrite(dirPin1, LOW);
    digitalWrite(dirPin2, HIGH);
  }

   bool motorCalibrationEnd = false;
   int numberStep = 0;
  
  while(motorCalibrationEnd == false){
    //Loop des enstops
    limitSwitch1.loop();
    limitSwitch2.loop();
    limitSwitch3.loop();
    limitSwitch4.loop();
    
    // 1 step per motor
    goMotor(stepPin1);
    goMotor(stepPin2);
    numberStep++;

    // if motor 1 arrives to the endstops, it moves away 4
    if(vertical){
      if (limitSwitch4.isPressed()) {
            digitalWrite(dirPin1, 1);
            digitalWrite(dirPin2, 1); 
            Serial.println("Button pressed, end calibration");
            moveAway();
            motorCalibrationEnd = true;
      }
    }else{

       if (limitSwitch2.isPressed()) {
            digitalWrite(dirPin1, 1);
            digitalWrite(dirPin2, 0); 
            Serial.println("Button pressed, end calibration");
            moveAway();
            motorCalibrationEnd = true;
        }
      }
    }

    return numberStep;
  
}
