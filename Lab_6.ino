//Lab 6

#include <LiquidCrystal.h>
#include <IRremote.hpp>

const int ldrPin = A0;
const int irPin = 7;

const int rs = 12;
const int en = 11;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const unsigned long BUTTON_1 = 0xEF10BF00;
const unsigned long BUTTON_2 = 0xEE11BF00;
const unsigned long BUTTON_3 = 0xED12BF00;
const unsigned long BUTTON_4 = 0xEB14BF00;

int displayMode = 1;

void setup() {
  Serial.begin(9600);

  pinMode(ldrPin, INPUT);


  lcd.begin(16, 2);


  lcd.setCursor(0, 0);
  lcd.print("Light Display");
  lcd.setCursor(0, 1);
  lcd.print("Ready");


  IrReceiver.begin(irPin);
}

void loop() {
  int lightLevel;


  lightLevel = analogRead(ldrPin);

  if (IrReceiver.decode()) {
    unsigned long buttonCode = IrReceiver.decodedIRData.decodedRawData;

    switch (buttonCode) {
      case BUTTON_1:
        displayMode = 1;
        break;
      case BUTTON_2:
        displayMode = 2;
        break;
      case BUTTON_3:
        displayMode = 3;
        break;
      case BUTTON_4:
        displayMode = 4;
        break;
      default:
        Serial.println(buttonCode, HEX);
        break;
    }

    IrReceiver.resume();
  }

  switch (displayMode) {
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("Light: ");
      lcd.print(lightLevel);
      break;
    case 2: {
      int percent = lightLevel * 100 / 1023;
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("Light: ");
      lcd.print(percent);
      lcd.print("%");
      break;
    }
    case 3:
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      if (lightLevel < 300) {
        lcd.print("Light: Dark");
      } else if (lightLevel < 700) {
        lcd.print("Light: Dim");
      } else {
        lcd.print("Light: Bright");
      }
      break;
    case 4:
      lcd.clear();
      break;
    default:
      break;
  }

  delay(150);
}
