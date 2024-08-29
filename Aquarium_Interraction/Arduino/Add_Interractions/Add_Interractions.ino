#include <SimpleTimer.h>  // Library for managing timers for random events
#include <ezButton.h>     // Library for handling endstops (limit switches)

// Declaration of endstops (limit switches) using ezButton library
ezButton limitSwitch1(42);  // Limit switch connected to pin 42
ezButton limitSwitch2(43);  // Limit switch connected to pin 43
ezButton limitSwitch3(41);  // Limit switch connected to pin 41
ezButton limitSwitch4(40);  // Limit switch connected to pin 40

// Declaration of stepper motor pins
#define stepPin1 2  // Step pin for motor 1
#define dirPin1 5   // Direction pin for motor 1
#define stepPin2 3  // Step pin for motor 2
#define dirPin2 6   // Direction pin for motor 2
#define stepPin3 4  // Step pin for motor 3
#define dirPin3 7   // Direction pin for motor 3

#define LENGTH 37  // Maximum length in X direction
#define HEIGHT 9   // Maximum height in Y direction

#define SPTR_SIZE 20  // Size of pointer array for storing strings
char *sPtr[SPTR_SIZE];  // Pointer array to store strings

// Declaration of timers for random variable changes
SimpleTimer timeTimer(8000);   // Timer with 8 seconds interval for random time events
SimpleTimer speedTimer(8000);  // Timer with 8 seconds interval for random speed events
SimpleTimer spinTimer(5000);   // Timer with 5 seconds interval for random spin events

// Default speed for stepper motors
int stepDelay = 200;  // Delay between steps in microseconds

// Variables to store the current position
int posX = 0;  // Current position in X direction
int posY = 0;  // Current position in Y direction

// Variables to store the current direction
// dirX: 1 for increase X, 0 for decrease X
bool dirX = 0;  
// dirY: 1 for increase Y, 0 for decrease Y
bool dirY = 0;  

bool stopMovement = false;

// Variables to store the maximum number of steps between 0 and max position for each direction
int stepNumberMotorHorizontal = 0;  // Max steps for horizontal movement
int stepNumberMotordirectionY = 0;  // Max steps for vertical movement

// Output string array to store incoming serial commands
String outputString[10];

void setup() {
  Serial.begin(9600);  // Initialize serial communication at 9600 baud rate

  // Set debounce time for each limit switch
  limitSwitch1.setDebounceTime(50);
  limitSwitch2.setDebounceTime(50);
  limitSwitch3.setDebounceTime(50);
  limitSwitch4.setDebounceTime(50);

  // Setup stepper motor pins as outputs
  pinMode(dirPin1, OUTPUT);
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin3, OUTPUT);
  pinMode(stepPin3, OUTPUT);

  // Set initial direction for all motors to HIGH
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, HIGH);
  digitalWrite(dirPin3, HIGH);

  // Calibrate direction Y and horizontal axis by moving motors to zero position
//  setMotorToZero(true);  // Calibrate Y direction
//  stepNumberMotordirectionY = motorCalibration(true);  // Get max steps for Y direction
//  Serial.print("Number of steps directionY :");
//  Serial.println(stepNumberMotordirectionY);
//
//  setMotorToZero(false);  // Calibrate X direction
//  stepNumberMotorHorizontal = motorCalibration(false);  // Get max steps for X direction
//  Serial.print("Number of steps horizontal :");
//  Serial.println(stepNumberMotorHorizontal);
}


void loop() {
  // Continuously read and process inputs

  // Read and update the state of each limit switch
  limitSwitch1.loop();
  limitSwitch2.loop();
  limitSwitch3.loop();
  limitSwitch4.loop();

  readInputData();  // Process any input from serial communication

  // Move each motor
  //goMotor(stepPin1);
  //goMotor(stepPin2);
  goMotor(stepPin3);

  // Check if any limit switch is pressed and change motor direction accordingly
  if (limitSwitch1.isPressed()) {
    setDirX(0);
    Serial.println("The button 1 is pressed");
    moveAway();
  }
  if (limitSwitch2.isPressed()) {
    setDirX(1);
    Serial.println("The button 2 is pressed");
    moveAway();
  }
  if (limitSwitch3.isPressed()) {
    setDirY(0);
    Serial.println("The button 3 is pressed");
    moveAway();
  }
  if (limitSwitch4.isPressed()) {
    setDirY(1);
    Serial.println("The button 4 is pressed");
    moveAway();
  }

  // Call random event functions
   aleaTime();
   aleaSpeed();
   aleaSpin();
}

// Function to move a stepper motor one step
void goMotor(int stepPin) {
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(stepDelay);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(stepDelay);
  updatePosition();  // Update the current position after each step
}

// Function to move away from limit switch when it is triggered
void moveAway() {
  for (int x = 0; x < 200; x++) {
    goMotor(stepPin1);
    goMotor(stepPin2);
  }
}

// Function to randomly change direction of stepper motors based on timer
void aleaTime() {
  int r = random(0, 3);  // Generate random number between 0 and 2
  r = r % 2;  // Convert to 0 or 1
  if (timeTimer.isReady() && r > 0) {  // If timer is ready and random number is 1
    digitalWrite(dirPin1, random(0, 2));  // Randomize direction for motor 1
    digitalWrite(dirPin2, random(0, 2));  // Randomize direction for motor 2
    timeTimer.reset();  // Reset the timer
  }
}
// Function to randomly change speed of stepper motors
void aleaSpeed() {
  int s = random(400, 1000);  // Generate random speed between 500 and 1000 microseconds
  if (speedTimer.isReady()) {  // If timer is ready
    stepDelay = s;  // Update the speed
    speedTimer.reset();  // Reset the timer
  }
}

// Function to manually set speed using timer
void setSpeedTimer(int s) {
  if (speedTimer.isReady()) {  // If timer is ready
    stepDelay = s;  // Update the speed
    speedTimer.reset();  // Reset the timer
  }
}

// Function to choose randomly spin direction of magnetic base
void setSpin(int d) {
  d = d % 2;  // Convert to 0 or 1
  if (spinTimer.isReady() && d > 0) {  // If timer is ready and random number is 1
    digitalWrite(dirPin3, random(0, 2));  // Randomize spin direction
    spinTimer.reset();  // Reset the timer
  }
}

// Function to randomly change spin direction using timer
void aleaSpin() {
  int d = random(0, 3);  // Generate random number between 0 and 2
  d = d % 2;  // Convert to 0 or 1
  if (spinTimer.isReady() && d > 0) {  // If timer is ready and random number is 1
    digitalWrite(dirPin3, random(0, 2));  // Randomize spin direction
    spinTimer.reset();  // Reset the timer
  }
  //stepDelay = random(150,800);  // Update the speed

}

void setMotorToZero(bool directionY) {
  // Set the initial direction for Y or X axis based on the parameter
  if (directionY) {
    setDirY(1);  // Set Y-axis direction to positive
  } else {
    setDirX(1);  // Set X-axis direction to positive
  }

  bool motorCalibrationEnd = false;  // Flag to indicate when calibration is complete

  // Calibration loop to move the motor until it reaches the endstop
  while (motorCalibrationEnd == false) {
    // Update endstop states
    limitSwitch1.loop();
    limitSwitch2.loop();
    limitSwitch3.loop();
    limitSwitch4.loop();

    // Perform one step for each motor
    goMotor(stepPin1);
    goMotor(stepPin2);

    // Check if endstop is pressed for the given direction
    if (directionY) {
      if (limitSwitch3.isPressed()) {
        setDirY(0);  // Reverse Y direction
        Serial.println("The button is pressed, 0 is set");
        posY = 0;  // Reset Y position
        moveAway();  // Move away from endstop
        motorCalibrationEnd = true;  // End calibration
      }
    } else {
      if (limitSwitch1.isPressed()) {
        setDirX(0);  // Reverse X direction
        Serial.println("The button is pressed, 0 is set");
        posX = 0;  // Reset X position
        moveAway();  // Move away from endstop
        motorCalibrationEnd = true;  // End calibration
      }
    }
  }
}

int motorCalibration(bool directionY) {
  // Set initial direction to move towards endstop
  if (directionY) {
    setDirY(0);  // Move Y in negative direction
  } else {
    setDirX(0);  // Move X in negative direction
  }

  bool motorCalibrationEnd = false;  // Flag for calibration completion
  int numberStep = 0;  // Counter for the number of steps taken

  // Loop until calibration is complete
  while (motorCalibrationEnd == false) {
    // Update endstop states
    limitSwitch1.loop();
    limitSwitch2.loop();
    limitSwitch3.loop();
    limitSwitch4.loop();

    // Perform one step for each motor and count steps
    goMotor(stepPin1);
    goMotor(stepPin2);
    numberStep++;

    // Check if the endstop is pressed to end calibration
    if (directionY) {
      if (limitSwitch4.isPressed()) {
        setDirY(1);  // Change Y direction
        Serial.println("Button pressed, end calibration");
        moveAway();  // Move away from endstop
        motorCalibrationEnd = true;  // End calibration
      }
    } else {
      if (limitSwitch2.isPressed()) {
        setDirX(1);  // Change X direction
        Serial.println("Button pressed, end calibration");
        moveAway();  // Move away from endstop
        motorCalibrationEnd = true;  // End calibration
      }
    }
  }

  return numberStep;  // Return the number of steps taken for calibration
}
void readInputData() {
  // Check if there is any incoming serial data
  if (Serial.available()) {

    String receivedString = "";  // Temporary string to hold received data
    receivedString = Serial.readStringUntil('\r\n');  // Read data until newline
    Serial.println(receivedString);
    // Convert received string to character buffer
    char buf[sizeof(receivedString)+1000];
    receivedString.toCharArray(buf, sizeof(buf)+1000);
    char *p = buf;
    char *str;
    int i = 0;

    // Split the received data using commas as delimiters
    while ((str = strtok_r(p, ",", &p)) != NULL) {
      Serial.println(str);  // Print each split segment
      outputString[i] = str;  // Store in outputString array
      i++;
    }
    Serial.println(outputString[0]);

    // Check the first command character and call the appropriate function
    if (outputString[0] == "C") {
      calibration();
    }
    if (outputString[0] == "I") {
      interractions();
    }
  }
 // goMotor(stepPin3);
}

void calibration() {
  Serial.println("Calibration");
  setSpeedTimer(200);  // Set speed for calibration
  goToZero();  // Move to zero position

  // Calculate number of steps needed for calibration on each axis
  int stepDirectionX = (outputString[1].toFloat() - 14) * stepNumberMotorHorizontal / LENGTH;
  int stepdirectionY = (outputString[2].toFloat() - 11) * stepNumberMotordirectionY / HEIGHT;
  Serial.println(stepDirectionX);
  Serial.println(stepdirectionY);

  // Move to the calculated positions
  goToPosition(true, 1,  stepdirectionY);
  goToPosition(false, 1, stepDirectionX);
}
void goToZero() {
  setDirY(0);  // Set initial direction to move towards zero position on Y-axis

  bool motorCalibrationEnd = false;  // Flag for calibration completion

  // Loop until zero position is reached
  while (motorCalibrationEnd == false) {
    // Update endstop states
    limitSwitch1.loop();
    limitSwitch2.loop();
    limitSwitch3.loop();
    limitSwitch4.loop();

    // Perform one step for each motor
    goMotor(stepPin1);
    goMotor(stepPin2);

    // Check if endstops are reached and adjust direction accordingly
    if (limitSwitch4.isPressed()) {
      setDirY(1);
      Serial.println("Button pressed, Stop");
      moveAway();
      setDirX(0);  // Change axis to X
    }
    if (limitSwitch2.isPressed()) {
      setDirX(1);
      Serial.println("Button pressed, Stop, end Zero");
      moveAway();
      motorCalibrationEnd = true;  // End calibration
    }
  }
}

void goToPosition(bool directionY, bool positive,  int stepNumber) {
  // Set the direction based on the Y or X axis
  if (directionY) {
    if(positive){
          setDirY(true);
    }
    else{
          setDirY(false);
    }
  } else {
    if(positive){
          setDirX(true);
    }
    else{
      setDirX(false);
    }
  }

  // Update endstop states
  limitSwitch1.loop();
  limitSwitch2.loop();
  limitSwitch3.loop();
  limitSwitch4.loop();

  // Loop to move the motor to the desired position
  for (int x = 0; x < stepNumber; x++) {
    // Update endstop states
    limitSwitch1.loop();
    limitSwitch2.loop();
    limitSwitch3.loop();
    limitSwitch4.loop();

    goMotor(stepPin1);
    goMotor(stepPin2);
    goMotor(stepPin3);

    // Check if any endstop is pressed during movement
    if (directionY) {
      if (limitSwitch3.isPressed()) {
        setDirY(0);
        Serial.println("The button 3 is pressed");
        moveAway();
        goto end;  // Exit loop if endstop is pressed
      }
      if (limitSwitch4.isPressed()) {
        setDirY(1);
        Serial.println("The button 4 is pressed");
        moveAway();
        goto end;  // Exit loop if endstop is pressed
      }
    } else {
      if (limitSwitch1.isPressed()) {
        setDirX(0);
        Serial.println("The button 1 is pressed");
        moveAway();
        goto end;  // Exit loop if endstop is pressed
      }
      if (limitSwitch2.isPressed()) {
        setDirX(1);
        Serial.println("The button 2 is pressed");
        moveAway();
        goto end;  // Exit loop if endstop is pressed
      }
    }
  }

end:
  Serial.println("Position reached");  // Print message when position is reached
  // save current dir and while current dir == old dir stop
}

void setDirX(int dir) {
  // Set the direction of movement for X-axis motors
    Serial.println("Set X dir");
  Serial.println(dir);
  if (dir==1) {
    digitalWrite(dirPin1, 1);
    digitalWrite(dirPin2, 0);
    dirX = 1;  // Update X direction state
    dirY = -1; // Invalidate Y direction state
  } else {
    digitalWrite(dirPin1, 0);
    digitalWrite(dirPin2, 1);
    dirX = 0;  // Update X direction state
    dirY = -1; // Invalidate Y direction state
  }
}

void setDirY(int dir) {
  // Set the direction of movement for Y-axis motors

  if (dir==1) {
    digitalWrite(dirPin1, 1);
    digitalWrite(dirPin2, 1);
    dirY = 1;  // Update Y direction state
    dirX = -1; // Invalidate X direction state
  } else {
    digitalWrite(dirPin1, 0);
    digitalWrite(dirPin2, 0);
    dirY = 0;  // Update Y direction state
    dirX = -1; // Invalidate X direction state
  }
}
void interractions() {
  Serial.println("Interraction");

  // Retrieve position and speed values from the input string
  // destX: Target position for X axis
  // destY: Target position for Y axis
  // speedMotor: Speed for Y axis
  String dirXTarget = outputString[1];
  int destX = outputString[2].toInt();
  String dirYTarget = outputString[3];
  int destY = outputString[4].toInt();
  String speedMotor = outputString[5];
  bool directionToGo;

  Serial.print("Pos X");
  Serial.print(destX);
  Serial.println();
  Serial.print("Pos Y");
  Serial.print(destY);
  Serial.println();
  Serial.print("Speed");
  Serial.print(speedMotor);
  Serial.println();
  
  // Determine the direction to move along the X axis
  if (dirXTarget == "M") {
    setDirX(0);  // Set direction to negative
    Serial.println("Negative X direction");
    directionToGo = 0;
  } else {
    setDirX(1);  // Set direction to positive
    Serial.println("Positive X direction");
    directionToGo = 1;

  }
  
  // Set motor speed 
  if(speedMotor=="L"){
    setSpeedTimer(700);
  }
  if(speedMotor=="M"){
    setSpeedTimer(400);
  }
  if(speedMotor =="V"){
     setSpeedTimer(150);
  }
  // Set X axis
  goToPosition(false, directionToGo, abs(destX));  // 'false' indicates movement along X axis

  // Determine the direction to move along the Y axis
  if (dirYTarget == "M") {
    setDirY(0);  // Set direction to negative
    Serial.println("Negative Y direction");
        directionToGo = 0;


  } else {
    setDirY(1);  // Set direction to positive
    Serial.println("Negative Y direction");
    directionToGo = 1;

  }

  // Set Y axis
  goToPosition(true, directionToGo, abs(destY));  // 'false' indicates movement along Y axis
  
  Serial.println("End Interraction");  // Indicate that the interaction process has ended
  Serial.flush();
}

void updatePosition() {
  // Update the current position values based on the current direction of the motors
  if (dirX == 1) {
    posX++;  // Increase X position if X direction is positive
  }
  if (dirX == 0) {
    posX--;  // Decrease X position if X direction is negative
  }
  if (dirY == 1) {
    posY++;  // Increase Y position if Y direction is positive
  }
  if (dirY == 0) {
    posY--;  // Decrease Y position if Y direction is negative
  }

  // Optionally print the updated positions for debugging purposes
  // Serial.println(posX);
  // Serial.println(posY);
}
