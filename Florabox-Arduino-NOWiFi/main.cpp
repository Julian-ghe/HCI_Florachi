#include <Arduino.h>
#include <LiquidCrystal.h>

double lastHealth = 0.0;

// --- LCD setup ---
const int rs = 11, en = 12, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int lcdContrastPin = 9;
const int lcdContrastValue = 10;

// --- Button pin ---
const int BTN = 8;

// --- Sensor pins ---
const int waterPin = A0;
const int tempPin  = A1;
const int lightPin = A2;

// --- Variables ---
int menuIndex = 0;
const int menuCount = 4;  // 0=main, 1=water, 2=light, 3=temp
bool displayOn = true;
unsigned long buttonPressStart = 0;
bool buttonPressed = false;

// --- Display update ---
void updateMenu() {
  lcd.clear();
  switch (menuIndex) {
    case 0: lcd.print("Main Screen"); lcd.setCursor(0,1); lcd.print("Plant Overview"); break;
    case 1: lcd.print("Water lvl: "); break;
    case 2: lcd.print("Light lvl: "); break;
    case 3: lcd.print("Temp: "); break;
  }
}

// --- Read and display sensor values ---
void updateReadings() {

  // Read raw sensor values
  int waterRaw = analogRead(waterPin);
  int lightRaw = analogRead(lightPin);
  int tempRaw = analogRead(tempPin);

  // Water level Display
  String water;
  String waterDesc;

  // Map to 0–100% scale, where 250 = no water, 390+ = drowning
  int waterInt = map(waterRaw, 250, 390, 0, 120);
  waterInt = constrain(waterInt, 0, 120);

  //Custom values
  if (0 <= waterInt && waterInt < 100) {
    water = String(waterInt);
    while (water.length() < 3) water = " " + water;
    water += " %";
  } 
  else if (waterInt >= 100 && waterInt < 110) {
    water = "100+%";
  }
  else {
    water = ">max ";
  }

  //Water Description
  if (waterInt < 10)
    waterDesc = "(No Water!!!)  ";
  else if (waterInt < 40)
    waterDesc = "(Dry)          ";
  else if (waterInt < 70)
    waterDesc = "(Okay)         ";
  else if (waterInt < 100)
    waterDesc = "(Good)         ";
  else if (waterInt < 110)
    waterDesc = "(Optimal)      ";
  else
    waterDesc = "(Drowning)     ";


  // Light level Display
  String light;
  String lightDesc;

  // Map to 0–100% scale, where 0 = dark, 430+ = very bright
  int lightInt = map(lightRaw, 0, 430, 100, 0);
  lightInt = constrain(lightInt, 0, 100);
  light = String(lightInt);
  while (light.length() < 3) light = " " + light;
  light += " %";

  // Light Description
  if (lightInt < 20) {
    lightDesc = "(Very Dim)     ";
  }
  else if (lightInt < 40) {
    lightDesc = "(Dim)          ";
  }
  else if (lightInt < 60) {
    lightDesc = "(Moderate)     ";
  }
  else if (lightInt < 80) {
    lightDesc = "(Bright)       ";
  }
  else {
    lightDesc = "(Ideal)        ";
  } 
  
  
  // Temperature Display
  String tempDesc;

  // Calculation of the voltage from the raw value
  double voltage = tempRaw * 5.0 / 1023.0;
  double temp = ((voltage / 5.0) * 10000.0) / (1.0 - (voltage / 5.0));
  temp = 1.0 / ((1.0 / 298.15) + (1.0 / 3950.0) * log(temp / 10000.0));
  temp = temp - 273.15;

  // Temperature Description
  if (temp < 15) {
    tempDesc = "(Freezing)       ";
  }
  else if (temp < 20) {
    tempDesc = "(Too Cold)       ";
  }
  else if (temp < 30) {
    tempDesc = "(Ideal)          ";
  }
  else if (temp < 35) {
    tempDesc = "(Too Warm)       ";
  }
  else {
    tempDesc = "(Cooked)         ";
  }

  //Total Display Update
  double lightDistance;
  if (lightInt >= 80) {
    lightDistance = 0;
  } else {
    lightDistance = (80 - lightInt)/80.0;
  }

  double tempDistance;
  if (temp >= 15 && temp <= 30) {
    tempDistance = 0;
  } else if (temp < 15) {
    tempDistance = (15 - temp)/15.0;
  } else if (temp <= 45) {
    tempDistance = (temp - 30)/15.0;
  }
  else {
    tempDistance = 1.0;
  }

  double waterDistance;
  if (110 > waterInt && waterInt >= 100) {
    waterDistance = 0;
  } else if (waterInt < 100) {
    waterDistance = (100 - waterInt)/100.0;
  } else {
    waterDistance = 1.0;
  }

  double totalDistance = 0.1 * lightDistance + 0.3 * tempDistance + 0.6 * waterDistance;
  double health = (1.0 - totalDistance) * 100.0;
  lastHealth = health;

  switch (menuIndex) {
    case 0:
      lcd.setCursor(0,0); lcd.print("You're Plant is: "); 
      lcd.setCursor(0,1); lcd.print(health); lcd.print(" % Healthy"); break;
    case 1:
      lcd.setCursor(11,0); lcd.print(water);
      lcd.setCursor(0,1); lcd.print(waterDesc); break;
    case 2:
      lcd.setCursor(11,0); lcd.print(light);
      lcd.setCursor(0,1); lcd.print(lightDesc); break;
    case 3:
      lcd.setCursor(7,0); lcd.print(temp); lcd.print(" C");
      lcd.setCursor(0,1); lcd.print(tempDesc); break;
  }
}

void setup() {
  // === LCD-Setup ===
  pinMode(lcdContrastPin, OUTPUT);
  analogWrite(lcdContrastPin, lcdContrastValue);
  lcd.begin(16, 2);
  lcd.print("Plant Monitor");
  delay(1000);
  lcd.clear();

  // === Sensor pins ===
  pinMode(lightPin, INPUT);
  pinMode(tempPin, INPUT);
  pinMode(waterPin, INPUT);

  // === Button pin ===
  pinMode(BTN, INPUT_PULLUP);
  Serial.begin(9600);
  updateMenu();
}

void loop() {
  bool btnState = digitalRead(BTN) == LOW; 

  if (btnState && !buttonPressed) {
    buttonPressed = true;
    buttonPressStart = millis();
  }

  // Button released
  if (!btnState && buttonPressed) {
    unsigned long pressDuration = millis() - buttonPressStart;
    buttonPressed = false;

    if (pressDuration < 5000) {
      // short press → next menu
      menuIndex = (menuIndex + 1) % menuCount;
      if (displayOn) updateMenu();
    }
  }

  // Long press while held down → toggle display
  if (btnState && (millis() - buttonPressStart > 5000)) {
    buttonPressed = false; // prevent re-trigger
    displayOn = !displayOn;
    lcd.clear();
    if (displayOn) {
      lcd.display();
      updateMenu();
    } else {
      lcd.noDisplay();
    }
  }

  // Update sensor values when display is on
  if (displayOn) {
    updateReadings();
    delay(500);
  }

  // Communicate health over Serial
  Serial.print("Health: ");
  Serial.println(lastHealth);
  delay(500);
}


