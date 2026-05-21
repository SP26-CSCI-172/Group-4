// Lab 7 - Line Following Robot with State Machine
// STUDENT STARTER
//
// Fill in the TODOs to complete the line-following state machine.
// Pin layout and speed constants are already provided - tested on the robot.
// Your job: define the states, set up the transition logic, and dispatch
// motor actions via switch/case.

// L298N motor driver pins
//TODO: update these pin numbers if you changed your wiring
#define ENA 9
#define IN1 12
#define IN2 11
#define ENB 6
#define IN3 7
#define IN4 8

// 5-channel IR line tracker pins (analog used as digital)
const int ir1 = A0;  // far left
const int ir2 = A1;  // mid left
const int ir3 = A2;  // center
const int ir4 = A3;  // mid right
const int ir5 = A4;  // far right

// Speed constants - leave as-is, tune at the end if you race
const int SPEED            = 80;
const int TURN_SPEED       = 100;
const int SHARP_TURN_SPEED = 150;

// TODO 1: define 6 state constants
//   STATE_CENTER, STATE_LEFT, STATE_SHARP_LEFT,
//   STATE_RIGHT,  STATE_SHARP_RIGHT, STATE_LOST
const int STATE_CENTER = 0;
const int STATE_LEFT = 1;
const int STATE_SHARP_LEFT = 2;
const int STATE_RIGHT = 3;
const int STATE_SHARP_RIGHT = 4;
const int STATE_LOST = 5;


// TODO 2: declare currentState and lastDirection global variables
//   int currentState = STATE_CENTER;
//   int lastDirection = 0;  // -1 = left, 0 = center, +1 = right
int currentState = STATE_CENTER;
int lastDirection = 0;  // -1 = left, 0 = center, +1 = right


void setup() {
  Serial.begin(9600);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);
  pinMode(ir3, INPUT);
  pinMode(ir4, INPUT);
  pinMode(ir5, INPUT);

  stopRobot();
  delay(2000);
}

// Motor helpers - already implemented for you. Truth table:
//   Forward       -> IN1=HIGH IN2=LOW   IN3=HIGH IN4=LOW
//   Soft turn L   -> IN1=LOW  IN2=LOW   IN3=HIGH IN4=LOW   (only right motor)
//   Soft turn R   -> IN1=HIGH IN2=LOW   IN3=LOW  IN4=LOW   (only left motor)
//   Sharp turn L  -> IN1=LOW  IN2=HIGH  IN3=HIGH IN4=LOW   (pivot)
//   Sharp turn R  -> IN1=HIGH IN2=LOW   IN3=LOW  IN4=HIGH  (pivot)
//   Stop          -> all LOW

void goForward() {
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
  //TODO: set motor direction for forward
  //   IN1=HIGH IN2=LOW   IN3=HIGH IN4=LOW
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnLeft() {
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);
  //TODO: set motor direction for soft left turn (only right motor moves)
  //   IN1=LOW  IN2=LOW   IN3=HIGH IN4=LOW
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);
  // TODO: set motor direction for soft right turn (only left motor moves)
  //   IN1=HIGH IN2=LOW   IN3=LOW  IN4=LOW
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void sharpTurnLeft() {
  analogWrite(ENA, SHARP_TURN_SPEED);
  analogWrite(ENB, SHARP_TURN_SPEED);
  //TODO: set motor direction for sharp left turn (pivot left)
  //   IN1=LOW  IN2=HIGH  IN3=HIGH IN4=LOW
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void sharpTurnRight() {
  analogWrite(ENA, SHARP_TURN_SPEED);
  analogWrite(ENB, SHARP_TURN_SPEED);
  //TODO: set motor direction for sharp right turn (pivot right)
  //   IN1=HIGH IN2=LOW   IN3=LOW  IN4=HIGH
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopRobot() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  //TODO: set motor direction for stop (all LOW)
  //   IN1=LOW  IN2=LOW   IN3=LOW  IN4=LOW
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void loop() {
  int s1 = digitalRead(ir1);
  int s2 = digitalRead(ir2);
  int s3 = digitalRead(ir3);
  int s4 = digitalRead(ir4);
  int s5 = digitalRead(ir5);

  Serial.print(s1);
  Serial.print(s2);
  Serial.print(s3);
  Serial.print(s4);
  Serial.println(s5);

  // TODO 3: TRANSITION - decide currentState from the sensor pattern
  //   Priority order: s3 first (center wins), then s2, s1, s4, s5
  //   Don't forget to update lastDirection when you set LEFT/RIGHT states
  if (s3 == HIGH) {
    currentState = STATE_CENTER;
    lastDirection = 0;
  }
  else if (s2 == HIGH) {
    currentState = STATE_LEFT;
    lastDirection = -1;
  }
  else if (s1 == HIGH) {
    currentState = STATE_SHARP_LEFT;
    lastDirection = -1;
  }
  else if (s4 == HIGH) {
    currentState = STATE_RIGHT;
    lastDirection = 1;
  }
  else if (s5 == HIGH) {
    currentState = STATE_SHARP_RIGHT;
    lastDirection = 1;
  }
  else {
    currentState = STATE_LOST;
  }

  // TODO 4: ACTION - switch/case on currentState
  //   STATE_CENTER      -> goForward();
  //   STATE_LEFT        -> turnLeft();
  //   STATE_SHARP_LEFT  -> sharpTurnLeft();
  //   STATE_RIGHT       -> turnRight();
  //   STATE_SHARP_RIGHT -> sharpTurnRight();
  //   STATE_LOST        -> recover using lastDirection
  //                          (-1 -> sharpTurnLeft, +1 -> sharpTurnRight, 0 -> stopRobot)
  switch (currentState) {
    case STATE_CENTER:
      goForward();
      break;
    case STATE_LEFT:
      turnLeft();
      break;
    case STATE_SHARP_LEFT:
      sharpTurnLeft();
      break;
    case STATE_RIGHT:
      turnRight();
      break;
    case STATE_SHARP_RIGHT:
      sharpTurnRight();
      break;
    case STATE_LOST:
      if (lastDirection == -1) {
        sharpTurnLeft();
      }
      else if (lastDirection == 1) {
        sharpTurnRight();
      }
      else {
        stopRobot();
      }
      break;
  }

  delay(5);
}
