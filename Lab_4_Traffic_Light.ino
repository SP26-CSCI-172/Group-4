// Lab 4 - Traffic Lights with Pedestrian Pushbutton

const int CAR_LIGHTS[] = {2, 3, 4};

const int CAR_RED = 0;
const int CAR_YELLOW = 1;
const int CAR_GREEN = 2;

const int PED_RED = 5;
const int PED_GREEN = 6;

const int BUTTON = 7;
bool walkRequested = false;

unsigned long lastPress = 0;
const long DEBOUNCE = 50;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 3; i++) {
    pinMode(CAR_LIGHTS[i], OUTPUT);
    digitalWrite(CAR_LIGHTS[i], LOW);
  }

  pinMode(PED_RED, OUTPUT);
  pinMode(PED_GREEN, OUTPUT);
  digitalWrite(PED_RED, HIGH);
  digitalWrite(PED_GREEN, LOW);

  pinMode(BUTTON, INPUT_PULLUP);
}

void checkButton() {
  if (digitalRead(BUTTON) == LOW) {
    if (millis() - lastPress >= DEBOUNCE) {
      walkRequested = true;
      lastPress = millis();
      Serial.println("Button pressed");
    }
  }
}

void pedestrianPhase() {
  digitalWrite(PED_RED, LOW);
  digitalWrite(PED_GREEN, HIGH);
  delay(5000);

  for (int i = 0; i < 10; i++) {
    digitalWrite(PED_GREEN, LOW);
    delay(500);
    digitalWrite(PED_GREEN, HIGH);
    delay(500);
  }

  digitalWrite(PED_GREEN, LOW);
  digitalWrite(PED_RED, HIGH);
}

void loop() {
  digitalWrite(CAR_LIGHTS[CAR_GREEN], HIGH);
  digitalWrite(CAR_LIGHTS[CAR_YELLOW], LOW);
  digitalWrite(CAR_LIGHTS[CAR_RED], LOW);
  for (int i = 0; i < 50; i++) {
    checkButton();
    delay(100);
  }

  digitalWrite(CAR_LIGHTS[CAR_GREEN], LOW);
  digitalWrite(CAR_LIGHTS[CAR_YELLOW], HIGH);
  for (int i = 0; i < 20; i++) {
    checkButton();
    delay(100);
  }
  
  digitalWrite(CAR_LIGHTS[CAR_YELLOW], LOW);
  digitalWrite(CAR_LIGHTS[CAR_RED], HIGH);
  for (int i = 0; i < 10; i++) {
    checkButton();
    delay(100);
  }

  if (walkRequested) {
    pedestrianPhase();
    walkRequested = false;
  }
  
  digitalWrite(CAR_LIGHTS[CAR_RED], LOW);
}
