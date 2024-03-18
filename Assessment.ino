#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4OLED display;
Zumo32U4LCD lcd;
Zumo32U4ButtonA buttonA;
Zumo32U4Buzzer buzzer;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;  
Zumo32U4ProximitySensors proxSensors;

// PREDEFINED VALUES 
#define QTR_THRESHOLD 400
#define SENSOR_THRESHOLD 6

#define REVERSE_SPEED 100
#define TURN_SPEED 100
#define FORWARD_SPEED 100
#define BACK_SPEED 170

#define TURN_DURATION 350
#define REVERSE_DURATION 350
#define NUM_SENSORS 3
#define DELAY 4000

// ROBOT VALUES
bool isMoving = false; // ROBOT MOVING DETECTION
uint16_t leftCounter = 0; // DEAD_END CALCULATION
uint16_t rightCounter = 0; // DEAD_END CALCULATION
uint16_t speed = FORWARD_SPEED; // MOTOR SPEED VALUE
unsigned int lineSensorValues[NUM_SENSORS];
unsigned long currentTime;

// COUNTDOWN
void countdown()
{
  ledYellow(1);

  buttonA.waitForButton(); // WAIT FOR USER TO PRESS START

  ledYellow(0);

  // PLAY MUSIC 
  for (int i = 0; i < 3; i++)
  {
    delay(1000);
    buzzer.playNote(NOTE_G(3), 200, 15);
  }

  delay(1000);
  buzzer.playNote(NOTE_G(4), 500, 15);
  delay(1000);

  isMoving = true; // ROBOT MOVING
}

// STOP ROBOT
void stop()
{
  motors.setSpeeds(0, 0);
  isMoving = false; // ROBOT STOPPED MOVING
}

// DISPLAY MESSAGE TO LCD
void displayMessage(int x, int y, const char *line){
  lcd.gotoXY(x, y);
  lcd.print(line); 
}

// DISPLAY SENSOR VALUES TO LCD
void displaySensorValues()
{
  lcd.clear();
  displayMessage(0, 1, String(lineSensorValues[0]).c_str()); // LEFT SENSOR
  displayMessage(2, 0, String(lineSensorValues[1]).c_str()); // MIDDLE SENSOR
  displayMessage(4, 1, String(lineSensorValues[2]).c_str()); // RIGHT SENSOR
  delay(20);
}

// REVERSE ROBOT
void reverse(int speed){
  motors.setSpeeds(-speed, -speed); // REVERSING
  delay(REVERSE_DURATION);
}

// TURN ROBOT
void turn(char direction, int speed){
  reverse(REVERSE_SPEED); // MOVE ROBOT BACKWARDS

  switch(direction){
    case 'L':
      motors.setSpeeds(-speed, speed); // TURNING LEFT
      break; 
    case 'R':
      motors.setSpeeds(speed, -speed); // TURNING RIGHT
      break;
  }

  delay(TURN_DURATION);
  motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED); // MOVE ROBOT FORWARD
}

// FOUND AN OBJECT 
void foundParcel(){
  buzzer.playNote(NOTE_G(3), 200, 15); // PLAY MUSIC
  ledYellow(1);
  lcd.clear(); 
  displayMessage(0, 0, "Parcel"); // DISPLAY A MESSAGE
  stop(); // STOP MOTORS
  delay(DELAY); // PAUSE ROBOT BREIFLY
  isMoving = true; // MOVING AGAIN
}

// CHECKS PROXIMITY AND SENDS ALLERT UPON OBJECT DETECTION
void proximityObjectDetection(){
  // DETECT PROXIMINITY DETECTORS
  int leftValue = proxSensors.countsLeftWithLeftLeds();
  int middleLeftValue = proxSensors.countsFrontWithLeftLeds();
  int middleRightValue = proxSensors.countsFrontWithRightLeds();
  int rightValue = proxSensors.countsRightWithLeftLeds(); 

  bool objectSeen = (middleLeftValue >= SENSOR_THRESHOLD) && (middleRightValue >= SENSOR_THRESHOLD) && (middleRightValue == middleLeftValue);

  if(objectSeen){
    foundParcel();
  } else if((leftValue == SENSOR_THRESHOLD) && (leftValue >= rightValue)) {
    // OBJECT SEEN ON LEFT SIDE, TURN THAT DIRECTION
    motors.setSpeeds(-90, 90); 
    delay(2000);
    motors.setSpeeds(0, 0); 
  } else if((rightValue == SENSOR_THRESHOLD) && (rightValue >= leftValue)) {
    // OBJECT SEEN ON RIGHT SIDE, TURN THAT DIRECTION
    motors.setSpeeds(90, -90); 
    delay(2000);
    motors.setSpeeds(0, 0); 
  } else {
    ledYellow(0);
    speed = FORWARD_SPEED; // NO OBJECT DETECTION, BACK TO NORMAL SPEED
  }
}

// GET OUT OF DEAD END 
void goBack(int speed){
  leftCounter = 0;
  rightCounter  = 0;
  motors.setSpeeds(speed, -speed); 
  delay(900);
  motors.setSpeeds(0, 0);
}

// MAKE SURE ROBOT IS CALIBRATED
void calibrateSensors()
{
  for(uint16_t i = 0; i < 120; i++)
  {
    if (i > 30 && i <= 90)
    {
      motors.setSpeeds(-200, 200);
    }
    else
    {
      motors.setSpeeds(200, -200);
    }

    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);
}

void setup()
{
  lcd.clear();
  displayMessage(0, 0, "Press A");

  lineSensors.initThreeSensors();
  proxSensors.initThreeSensors(); 

  calibrateSensors();

  countdown();

  currentTime = millis();
}

void loop()
{
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
  proxSensors.read();

  proximityObjectDetection(); // DETECT OBJECT

  displaySensorValues();  // DISPLAY LINE SENSORS

  if (buttonA.isPressed())
  {
    stop();  // BUTTON PRESSED, STOP ROBOT
    countdown(); // GO BACK TO COUNTDOWN 
    currentTime = millis();
  }

  int borderDetect = lineSensorValues[0] > QTR_THRESHOLD || lineSensorValues[1] > QTR_THRESHOLD || lineSensorValues[2] > QTR_THRESHOLD;
  
  if(borderDetect && isMoving)
  {
    long distance = millis() - currentTime;
    if(leftCounter >= 5 && rightCounter >= 5 && distance >= 3000){
      goBack(BACK_SPEED); // DEAD END
      currentTime = millis(); // RESET DISTANCE TIMER
    } 

    if(lineSensorValues[2] > QTR_THRESHOLD)
    {
      turn('L', TURN_SPEED); // LEFT TURN 
      leftCounter++;// INCREMENT DEAD END COUNTER
    }  

    if(lineSensorValues[0] > QTR_THRESHOLD)
    {
     turn('R', TURN_SPEED); // RIGHT TURN
     rightCounter++; // INCREMENT DEAD END COUNTER
    } 

  } else {
    motors.setSpeeds(speed, speed); // FORWARD
  }
}
