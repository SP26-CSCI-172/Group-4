
#include <Servo.h>
#include <IRremote.hpp>

//Motor driver pins
const int ENA = 3;
const int IN1 = 12;
const int IN2 = 11;
const int ENB = 6;
const int IN3 = 7;
const int IN4 = 8;

//Sensor and output pins
const int LEFT_IR_SENSOR = 2;
const int RIGHT_IR_SENSOR = 4;
const int TRIG_PIN = 5;
const int ECHO_PIN = A5;
const int SERVO_PIN = 9;
const int IR_RECEIVE_PIN = 10;
const int BUZZER_PIN = 13;

const int TRACKER_S1 = A0;
const int TRACKER_S2 = A1;
const int TRACKER_S3 = A2;
const int TRACKER_S4 = A3;
const int TRACKER_S5 = A4;

//IR remote command values 
const int IR_UP = 24;
const int IR_DOWN = 82;
const int IR_LEFT = 8;
const int IR_RIGHT = 90;
const int IR_OK = 28;
const int IR_STAR = 22;
const int IR_HASH = 13;
const int IR_1 = 69;
const int IR_2 = 70;
const int IR_3 = 71;
const int IR_4 = 68;
const int IR_5 = 64;
const int IR_0 = 25;

//Speed and distance settings
const int MANUAL_SPEED = 180;
const int PATROL_SPEED = 135;
const int TURN_SPEED = 160;
const int FOLLOW_SPEED = 120;
const int LINE_SPEED = 90;
const int LINE_TURN_SPEED = 130;
const int LINE_SHARP_TURN_SPEED = 130;

const int TOO_CLOSE_CM = 14;
const int OBSTACLE_CM = 28;
const int FOLLOW_TOO_CLOSE_CM = 18;
const int FOLLOW_STOP_MIN_CM = 25;
const int FOLLOW_GO_CM = 38;
const int FOLLOW_DETECT_MAX_CM = 120;

const int OBJECT_DETECTED = LOW;

const int TRACKER_ACTIVE = HIGH;

const int BUZZER_ON = LOW;
const int BUZZER_OFF = HIGH;

enum RobotMode {
  MODE_STOPPED,
  MODE_MANUAL,
  MODE_PATROL,
  MODE_FOLLOW,
  MODE_GUARD,
  MODE_LINE_FOLLOW
};

const int LINE_STATE_CENTER = 0;
const int LINE_STATE_LEFT = 1;
const int LINE_STATE_SHARP_LEFT = 2;
const int LINE_STATE_RIGHT = 3;
const int LINE_STATE_SHARP_RIGHT = 4;
const int LINE_STATE_LOST = 5;

RobotMode currentMode = MODE_STOPPED;
Servo scanner;

unsigned long lastDebugPrint = 0;
unsigned long lastGuardAlarm = 0;
unsigned long hornStopTime = 0;
bool hornActive = false;
int lineCurrentState = LINE_STATE_CENTER;
int lineLastDirection = 0;

void setMotorSpeed(int leftSpeed, int rightSpeed) {
  if (rightSpeed > 0) {
    digitalWrite(ENA, HIGH);
  }
  else {
    digitalWrite(ENA, LOW);
  }

  analogWrite(ENB, leftSpeed);
}

void setup() {
  Serial.begin(9600);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(LEFT_IR_SENSOR, INPUT);
  pinMode(RIGHT_IR_SENSOR, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, BUZZER_OFF);

  pinMode(TRACKER_S1, INPUT);
  pinMode(TRACKER_S2, INPUT);
  pinMode(TRACKER_S3, INPUT);
  pinMode(TRACKER_S4, INPUT);
  pinMode(TRACKER_S5, INPUT);

  scanner.attach(SERVO_PIN);
  scanner.write(90);

  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);

  stopRobot();
  startupSelfTest();
  setMode(MODE_STOPPED);
}

void loop() {
  handleRemote();
  updateHorn();
  printDebugInfo();

  if (currentMode == MODE_PATROL) {
    patrolMode();
  }
  else if (currentMode == MODE_FOLLOW) {
    followMode();
  }
  else if (currentMode == MODE_GUARD) {
    guardMode();
  }
  else if (currentMode == MODE_LINE_FOLLOW) {
    lineFollowMode();
  }
}

// Remote control

void handleRemote() {
  if (!IrReceiver.decode()) {
    return;
  }

  int command = IrReceiver.decodedIRData.command;
  Serial.print("IR command: ");
  Serial.println(command);

  if (command == IR_OK) {
    setMode(MODE_STOPPED);
  }
  else if (command == IR_0) {
    startHorn();
  }
  else if (command == IR_1) {
    setMode(MODE_MANUAL);
  }
  else if (command == IR_2) {
    setMode(MODE_PATROL);
  }
  else if (command == IR_3) {
    setMode(MODE_FOLLOW);
  }
  else if (command == IR_4) {
    setMode(MODE_GUARD);
  }
  else if (command == IR_5) {
    setMode(MODE_LINE_FOLLOW);
  }
  else if (command == IR_STAR) {
    runSensorDemo();
  }
  else if (command == IR_HASH) {
    scanner.write(90);
    beep(2, 80);
  }
  else if (currentMode == MODE_MANUAL) {
    handleManualDrive(command);
  }

  IrReceiver.resume();
}

void handleManualDrive(int command) {
  if (command == IR_UP) {
    if (frontIsBlocked()) {
      stopRobot();
      warningBeep();
    }
    else {
      goForward(MANUAL_SPEED);
    }
  }
  else if (command == IR_DOWN) {
    goBackward(MANUAL_SPEED);
  }
  else if (command == IR_LEFT) {
    pivotLeft(TURN_SPEED);
  }
  else if (command == IR_RIGHT) {
    pivotRight(TURN_SPEED);
  }
}

void setMode(RobotMode newMode) {
  currentMode = newMode;
  stopRobot();
  scanner.write(90);

  if (currentMode == MODE_STOPPED) {
    Serial.println("Mode: STOPPED");
    beep(1, 80);
  }
  else if (currentMode == MODE_MANUAL) {
    Serial.println("Mode: MANUAL DRIVE");
    beep(1, 180);
  }
  else if (currentMode == MODE_PATROL) {
    Serial.println("Mode: AUTONOMOUS PATROL");
    beep(2, 120);
  }
  else if (currentMode == MODE_FOLLOW) {
    Serial.println("Mode: FOLLOW OBJECT");
    beep(3, 100);
  }
  else if (currentMode == MODE_GUARD) {
    Serial.println("Mode: GUARD ALARM");
    beep(4, 80);
  }
  else if (currentMode == MODE_LINE_FOLLOW) {
    Serial.println("Mode: LINE FOLLOW");
    lineCurrentState = LINE_STATE_CENTER;
    lineLastDirection = 0;
    beep(5, 60);
  }
}

//Main robot behaviors

void patrolMode() {
  if (floorMarkerDetected()) {
    stopRobot();
    Serial.println("Floor marker/edge detected. Backing away.");
    warningBeep();
    goBackward(130);
    delay(450);
    pivotRight(TURN_SPEED);
    delay(450);
    stopRobot();
    return;
  }

  if (frontIsBlocked()) {
    stopRobot();
    warningBeep();
    avoidObstacleWithScan();
    return;
  }

  goForward(PATROL_SPEED);
  delay(40);
}

void followMode() {
  int leftIR = digitalRead(LEFT_IR_SENSOR);
  int rightIR = digitalRead(RIGHT_IR_SENSOR);
  int distance = readDistanceCM();

  bool frontObjectSeen = (distance > 0 && distance <= FOLLOW_DETECT_MAX_CM);

  if (!frontObjectSeen) {
    stopRobot();
  }
  else if (distance < FOLLOW_TOO_CLOSE_CM) {
    stopRobot();
    goBackward(95);
    delay(120);
    stopRobot();
  }
  else if (distance >= FOLLOW_STOP_MIN_CM && distance <= FOLLOW_GO_CM) {
    stopRobot();
  }
  else if (leftIR == OBJECT_DETECTED && rightIR != OBJECT_DETECTED) {
    pivotLeft(105);
  }
  else if (leftIR != OBJECT_DETECTED && rightIR == OBJECT_DETECTED) {
    pivotRight(105);
  }
  else if (distance > FOLLOW_GO_CM) {
    goForward(FOLLOW_SPEED);
  }
  else {
    stopRobot();
  }

  delay(50);
}

void guardMode() {
  stopRobot();

  int centerDistance = readDistanceCM();
  int leftIR = digitalRead(LEFT_IR_SENSOR);
  int rightIR = digitalRead(RIGHT_IR_SENSOR);

  if ((centerDistance > 0 && centerDistance < OBSTACLE_CM) ||
      leftIR == OBJECT_DETECTED ||
      rightIR == OBJECT_DETECTED ||
      floorMarkerDetected()) {
    unsigned long now = millis();
    if (now - lastGuardAlarm > 1000) {
      Serial.println("Guard alarm: object or marker detected.");
      alarmBeep();
      lastGuardAlarm = now;
    }
  }

  scanner.write(60);
  delay(120);
  scanner.write(120);
  delay(120);
  scanner.write(90);
  delay(120);
}

void lineFollowMode() {
  int s1 = digitalRead(TRACKER_S1);
  int s2 = digitalRead(TRACKER_S2);
  int s3 = digitalRead(TRACKER_S3);
  int s4 = digitalRead(TRACKER_S4);
  int s5 = digitalRead(TRACKER_S5);

  Serial.print(s1);
  Serial.print(s2);
  Serial.print(s3);
  Serial.print(s4);
  Serial.println(s5);

  if (s3 == HIGH) {
    lineCurrentState = LINE_STATE_CENTER;
    lineLastDirection = 0;
  }
  else if (s2 == HIGH) {
    lineCurrentState = LINE_STATE_LEFT;
    lineLastDirection = -1;
  }
  else if (s1 == HIGH) {
    lineCurrentState = LINE_STATE_SHARP_LEFT;
    lineLastDirection = -1;
  }
  else if (s4 == HIGH) {
    lineCurrentState = LINE_STATE_RIGHT;
    lineLastDirection = 1;
  }
  else if (s5 == HIGH) {
    lineCurrentState = LINE_STATE_SHARP_RIGHT;
    lineLastDirection = 1;
  }
  else {
    lineCurrentState = LINE_STATE_LOST;
  }

  switch (lineCurrentState) {
    case LINE_STATE_CENTER:
      lineGoForward();
      break;
    case LINE_STATE_LEFT:
      lineTurnLeft();
      break;
    case LINE_STATE_SHARP_LEFT:
      lineSharpTurnLeft();
      break;
    case LINE_STATE_RIGHT:
      lineTurnRight();
      break;
    case LINE_STATE_SHARP_RIGHT:
      lineSharpTurnRight();
      break;
    case LINE_STATE_LOST:
      if (lineLastDirection == -1) {
        lineSharpTurnLeft();
      }
      else if (lineLastDirection == 1) {
        lineSharpTurnRight();
      }
      else {
        stopRobot();
      }
      break;
  }

  delay(5);
}

void avoidObstacleWithScan() {
  goBackward(130);
  delay(350);
  stopRobot();

  int leftDistance = scanAtAngle(150);
  int centerDistance = scanAtAngle(90);
  int rightDistance = scanAtAngle(30);

  Serial.print("Scan L/C/R: ");
  Serial.print(leftDistance);
  Serial.print(" / ");
  Serial.print(centerDistance);
  Serial.print(" / ");
  Serial.println(rightDistance);

  if (leftDistance > rightDistance && leftDistance > OBSTACLE_CM) {
    pivotLeft(TURN_SPEED);
    delay(500);
  }
  else if (rightDistance >= leftDistance && rightDistance > OBSTACLE_CM) {
    pivotRight(TURN_SPEED);
    delay(500);
  }
  else {
    pivotRight(TURN_SPEED);
    delay(850);
  }

  stopRobot();
  scanner.write(90);
}

void slowSearch() {
  pivotLeft(95);
  delay(160);
  stopRobot();
}

void runSensorDemo() {
  stopRobot();
  Serial.println("Running sensor demo.");
  beep(2, 70);

  int leftDistance = scanAtAngle(150);
  int centerDistance = scanAtAngle(90);
  int rightDistance = scanAtAngle(30);

  Serial.print("Demo distances L/C/R: ");
  Serial.print(leftDistance);
  Serial.print(" / ");
  Serial.print(centerDistance);
  Serial.print(" / ");
  Serial.println(rightDistance);

  Serial.print("Obstacle L/R: ");
  Serial.print(digitalRead(LEFT_IR_SENSOR));
  Serial.print(" / ");
  Serial.println(digitalRead(RIGHT_IR_SENSOR));

  printTrackerValues();
  scanner.write(90);
  beep(3, 70);
}

// Sensor helpers

bool frontIsBlocked() {
  int distance = readDistanceCM();
  bool ultrasonicBlocked = (distance > 0 && distance < OBSTACLE_CM);
  bool leftBlocked = (digitalRead(LEFT_IR_SENSOR) == OBJECT_DETECTED);
  bool rightBlocked = (digitalRead(RIGHT_IR_SENSOR) == OBJECT_DETECTED);
  return ultrasonicBlocked || leftBlocked || rightBlocked;
}

int readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  if (duration == 0) {
    return 0;
  }

  return duration / 58;
}

int scanAtAngle(int angle) {
  scanner.write(angle);
  delay(350);
  return readDistanceCM();
}

bool floorMarkerDetected() {
  int s1 = digitalRead(TRACKER_S1);
  int s2 = digitalRead(TRACKER_S2);
  int s3 = digitalRead(TRACKER_S3);
  int s4 = digitalRead(TRACKER_S4);
  int s5 = digitalRead(TRACKER_S5);

  int activeCount = 0;
  if (s1 == TRACKER_ACTIVE) activeCount++;
  if (s2 == TRACKER_ACTIVE) activeCount++;
  if (s3 == TRACKER_ACTIVE) activeCount++;
  if (s4 == TRACKER_ACTIVE) activeCount++;
  if (s5 == TRACKER_ACTIVE) activeCount++;

  return activeCount >= 3;
}

void printTrackerValues() {
  Serial.print("Tracker S1-S5: ");
  Serial.print(digitalRead(TRACKER_S1));
  Serial.print(" ");
  Serial.print(digitalRead(TRACKER_S2));
  Serial.print(" ");
  Serial.print(digitalRead(TRACKER_S3));
  Serial.print(" ");
  Serial.print(digitalRead(TRACKER_S4));
  Serial.print(" ");
  Serial.println(digitalRead(TRACKER_S5));
}

void printDebugInfo() {
  unsigned long now = millis();
  if (now - lastDebugPrint < 500) {
    return;
  }

  Serial.print("Mode: ");
  Serial.print(currentMode);
  Serial.print("  Distance: ");
  Serial.print(readDistanceCM());
  Serial.print("  IR L/R: ");
  Serial.print(digitalRead(LEFT_IR_SENSOR));
  Serial.print("/");
  Serial.print(digitalRead(RIGHT_IR_SENSOR));
  Serial.print("  Floor marker: ");
  Serial.println(floorMarkerDetected());

  lastDebugPrint = now;
}

//Motor helpers

void goForward(int speedValue) {
  setMotorSpeed(speedValue, speedValue);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void goBackward(int speedValue) {
  setMotorSpeed(speedValue, speedValue);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void pivotLeft(int speedValue) {
  setMotorSpeed(speedValue, speedValue);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void pivotRight(int speedValue) {
  setMotorSpeed(speedValue, speedValue);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopRobot() {
  setMotorSpeed(0, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void lineGoForward() {
  setMotorSpeed(LINE_SPEED, LINE_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void lineTurnLeft() {
  setMotorSpeed(LINE_TURN_SPEED, LINE_TURN_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void lineTurnRight() {
  setMotorSpeed(LINE_TURN_SPEED, LINE_TURN_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void lineSharpTurnLeft() {
  setMotorSpeed(LINE_SHARP_TURN_SPEED, LINE_SHARP_TURN_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void lineSharpTurnRight() {
  setMotorSpeed(LINE_SHARP_TURN_SPEED, LINE_SHARP_TURN_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// Buzzer helpers

void startHorn() {
  hornActive = true;
  hornStopTime = millis() + 2000;
  digitalWrite(BUZZER_PIN, BUZZER_ON);
  Serial.println("Horn on");
}

void updateHorn() {
  if (hornActive && millis() >= hornStopTime) {
    hornActive = false;
    digitalWrite(BUZZER_PIN, BUZZER_OFF);
    Serial.println("Horn off");
  }
}

void beep(int count, int durationMs) {
  if (hornActive) {
    return;
  }

  for (int i = 0; i < count; i++) {
    digitalWrite(BUZZER_PIN, BUZZER_ON);
    delay(durationMs);
    digitalWrite(BUZZER_PIN, BUZZER_OFF);
    delay(durationMs);
  }
  digitalWrite(BUZZER_PIN, BUZZER_OFF);
}

void warningBeep() {
  beep(2, 90);
}

void alarmBeep() {
  if (hornActive) {
    return;
  }

  for (int i = 0; i < 4; i++) {
    digitalWrite(BUZZER_PIN, BUZZER_ON);
    delay(70);
    digitalWrite(BUZZER_PIN, BUZZER_OFF);
    delay(45);
  }
  digitalWrite(BUZZER_PIN, BUZZER_OFF);
}

void startupSelfTest() {
  Serial.println("Startup self-test");
  beep(1, 100);

  scanner.write(45);
  delay(250);
  scanner.write(135);
  delay(250);
  scanner.write(90);
  delay(250);

  printTrackerValues();
  Serial.print("Distance cm: ");
  Serial.println(readDistanceCM());
  Serial.print("IR obstacle L/R: ");
  Serial.print(digitalRead(LEFT_IR_SENSOR));
  Serial.print(" / ");
  Serial.println(digitalRead(RIGHT_IR_SENSOR));

  beep(2, 80);
}
