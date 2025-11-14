#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "eyes.h"
#include <WiFi.h>  // ESP32 WLAN-Client
#include <HTTPClient.h>

//WLAN-Zugangsdaten (bitte anpassen)
const char* ssid = "SSID"; //SSID
const char* password = "PASSWORD"; //Passwort

//Serveradresse des Raspberry Pis
const char* serverIP = "10.247.67.82"; // anpassen
const int serverPort = 80;

//OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Buzzer
#define BUZZER_PIN 25
#define BUZZER_CHANNEL 0

//Ultraschallsensor HC-SR04
#define TRIG_PIN 5
#define ECHO_PIN 18
const int GESTURE_THRESHOLD_MIN = 10;  // min. 10 cm
const int GESTURE_THRESHOLD_MAX = 20;  // max. 20 cm
const int BUZZER_TOGGLE_RANGE = 5;     // Doppelbewegung unter 5 cm

//Zeitkonstanten
#define OLED_TIMEOUT_MS 5000
#define MEASURE_INTERVAL_MS 100
#define GESTURE_COOLDOWN_MS 2000
const unsigned long DOUBLE_WAVE_TIMEOUT = 600; // Zeitfenster für zweite Welle (ms)
const unsigned long WAVE_DEBOUNCE_MS = 150;    // Entprellzeit

//Zustände
bool oledActive = false;
unsigned long oledTimerStart = 0;
unsigned long lastMeasureTime = 0;
unsigned long lastGestureTime = 0;

//Soundsteuerung
bool buzzerEnabled = true; // true = Sound an, false = stumm
float lastHealth = -1; // -1 = ungültig

//Notenfrequenzen
#define C4 262
#define D4 294
#define E4 330
#define F4 349
#define G4 392
#define A4 440
#define B4 494
#define C5 523
#define D5 587
#define E5 659
#define G5 784

//Notenfrequenzen (zweites Set)
#define Db5 622
#define B4 494
#define D5 587
#define C5 523
#define A4 440
#define E4 330
#define Gb4 415
#define F5 698

//Funktionsprototypen
void playTone(int frequency, int duration);
void playMelody(String mood);
long measureDistanceCM();
float readHealth();
void updateEmotion(float health);

//Tonfunktion
void playTone(int frequency, int duration) {
  ledcWriteTone(BUZZER_CHANNEL, frequency);
  delay(duration);
  ledcWriteTone(BUZZER_CHANNEL, 0);
  delay(30); // kleine Pause
}

//Melodien je nach Stimmung
void playMelody(String mood) {
  if (mood == "cry") {
    playTone(150, 400);
    delay(100);
    playTone(120, 400);
  } else if (mood == "sad") {
    playTone(G4, 250);
    playTone(F4, 250);
    playTone(E4, 400);
  } else if (mood == "normal") {
    playTone(A4, 200);
    delay(50);
    playTone(A4, 200);
    delay(50);
    playTone(A4, 200);
  } else if (mood == "happy") {
    playTone(C5, 150);
    playTone(E5, 150);
    playTone(G5, 200);
    delay(100);
    playTone(E5, 150);
    playTone(G5, 300);
  } else if (mood == "super_happy") {
    playTone(C5, 150);
    playTone(D5, 150);
    playTone(E5, 150);
    playTone(G5, 150);
    playTone(E5, 150);
    playTone(G5, 300);
    delay(100);
    playTone(C5, 150);
    playTone(E5, 150);
    playTone(G5, 400);
  } else {
    //stumm
  }
}

//Ultraschallmessung
long measureDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;
  return duration / 58.2;
}

// Health vom UNO abrufen
float readHealth() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String("http://") + serverIP + ":" + serverPort;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Antwort vom UNO:");
      Serial.println(payload);
      int index = payload.indexOf("Health:");
      if (index != -1) {
        String valueStr = payload.substring(index + 7);
        valueStr.trim();
        return valueStr.toFloat();
      }
    } else {
      Serial.print("HTTP Fehler: ");
      Serial.println(http.errorToString(httpCode));
    }
    http.end();
  } else {
    Serial.println("WLAN nicht verbunden");
  }
  return -1;
}

//Health löst Emotionen aus
void updateEmotion(float health) {
  health = constrain(health, 0, 100);

  if (health >= 0 && health <= 20) {
    cry(health);
    if (buzzerEnabled) playMelody("cry");
  } else if (health >= 21 && health <= 39) {
    sad(health);
    if (buzzerEnabled) playMelody("sad");
  } else if (health >= 40 && health <= 59) {
    normal(health);
    if (buzzerEnabled) playMelody("normal");
  } else if (health >= 60 && health <= 79) {
    happy(health);
    if (buzzerEnabled) playMelody("happy");
  } else if (health >= 80 && health <= 100) {
    super_happy(health);
    if (buzzerEnabled) playMelody("super_happy");
  } else {
    Serial.println("Ungültiger Health-Wert in updateEmotion()");
  }
}

void setup() {
  Serial.begin(115200);

  //WLAN
  WiFi.begin(ssid, password);
  Serial.print("Verbinde mit WLAN");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n WLAN verbunden!");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());

  //Pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  //Buzzer
  ledcSetup(BUZZER_CHANNEL, 1000, 8);
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);

  //OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init fehlgeschlagen");
    while (true);
  }

  initEyes(display);
  display.ssd1306_command(SSD1306_DISPLAYON);

  //Health-Wert abrufen
  float health = -1;
  const int maxRetries = 5;

  for (int i = 0; i < maxRetries; i++) {
    Serial.print("Versuche Health-Wert abzurufen... Versuch ");
    Serial.println(i + 1);
    health = readHealth();
    if (health >= 0) break;
    delay(1000);
  }

  if (health >= 0) {
    Serial.print("Health (beim Start): ");
    Serial.println(health);
    updateEmotion(health);
    lastHealth = health;
    oledTimerStart = millis();
    oledActive = true;
  } else {
    Serial.println("Health konnte beim Start nicht abgerufen werden.");
    oledActive = false;
  }
}
//LOOP
void loop() {
  unsigned long now = millis();

  //Änderung: Messung nur alle 100 ms
  if (now - lastMeasureTime >= MEASURE_INTERVAL_MS) {
    lastMeasureTime = now;
    long distance = measureDistanceCM();

    //GESTENERKENNUNG
    static bool handWasVeryClose = false;
    static unsigned long lastWaveTime = 0;
    static int waveCount = 0;

    bool handIsVeryClose = (distance > 0 && distance <= BUZZER_TOGGLE_RANGE);
    bool handIsMedium = (distance >= GESTURE_THRESHOLD_MIN && distance <= GESTURE_THRESHOLD_MAX);

    //Änderung: Welle erkannt
    if (handIsVeryClose && !handWasVeryClose) {
      if (now - lastWaveTime > WAVE_DEBOUNCE_MS) {
        waveCount++;
        lastWaveTime = now;
        Serial.print("Welle erkannt #");
        Serial.println(waveCount);
      }
    }
    handWasVeryClose = handIsVeryClose;

    //Doppelgeste erkannt (<5 cm)
    if (waveCount == 2 && (now - lastWaveTime < DOUBLE_WAVE_TIMEOUT)) {
      buzzerEnabled = !buzzerEnabled; // Sound AN/AUS
      Serial.println(buzzerEnabled ? "Sound EIN" : "Sound AUS");

      //OLED-Anzeige für Sound-Status
      display.ssd1306_command(SSD1306_DISPLAYON);
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(15, 20);
      display.println(buzzerEnabled ? "Sound ON" : "Sound OFF");
      display.display();

      //optionaler Bestätigungston
      if (buzzerEnabled) playTone(800, 150);
      else playTone(200, 150);

      oledActive = true;
      oledTimerStart = now;

      //Reset der Zählung
      waveCount = 0;
      lastWaveTime = now;
    }

    //Timeout für zweite Welle
    if (waveCount == 1 && (now - lastWaveTime > DOUBLE_WAVE_TIMEOUT)) {
      waveCount = 0;
    }

    //Normale Geste (10–20 cm): OLED aktivieren
    if (handIsMedium) {
      if (now - lastGestureTime > GESTURE_COOLDOWN_MS) {
        Serial.print("Einfache Geste erkannt bei ");
        Serial.print(distance);
        Serial.println(" cm");

        display.ssd1306_command(SSD1306_DISPLAYON);

        float currentHealth = readHealth();
        if (currentHealth >= 0) {
          lastHealth = currentHealth;
          updateEmotion(currentHealth);
        } else if (lastHealth >= 0) {
          updateEmotion(lastHealth);
        } else {
          display.clearDisplay();
          display.display();
        }

        oledActive = true;
        oledTimerStart = now;
        lastGestureTime = now;
      }
    }
  }

  //OLED Timeout
  if (oledActive && (now - oledTimerStart > OLED_TIMEOUT_MS)) {
    Serial.println("OLED Timeout – Display aus");
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    oledActive = false;
  }

  //Health regelmässig prüfen (alle 10 s)
  static unsigned long lastHealthCheck = 0;
  const unsigned long HEALTH_CHECK_INTERVAL = 10000;
  if (now - lastHealthCheck >= HEALTH_CHECK_INTERVAL) {
    lastHealthCheck = now;
    float health = readHealth();
    if (health >= 0) {
      lastHealth = health;
      updateEmotion(health);
      oledActive = true;
      oledTimerStart = now;
    } else if (lastHealth >= 0) {
      updateEmotion(lastHealth);
      oledActive = true;
      oledTimerStart = now;
    }
  }

  delay(10); // kleine Pause
}
