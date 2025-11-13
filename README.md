# HCI_Florachi

Projekt Beschreibung

WICHTIGE Anmerkung: Auf grund einer Restriktion benutzen wir anstatt eine Adruino Uno WIFI, einen Adruino R3 und Rasberry Pi 3 Model B zur Kommunikation, wenn ein Adruino Wifi vorrätig ist wär dies die einfachste Lösung.

Harware Setup:
- Florabox
    - Komponeten:
          - Arduino Uno (No WiFi)
          - Rasberry Pi (WiFi)
          - KY-013 Temperatur Sensor
          - KY-018 Licht Sensor
          - Arduino Wassersensor Modul
          - Arduino LCD 1602 Display
          - KY-004 Button
          - Arduino Breadboard
    - Verkablung:
          - WICHTIGE Anmerkung: Wenn ein Rasberry Pi Angeschlossen wird, dann wird der Arduino mit USB zum Rasberry Pi verbunden.
          ![Verkaubkung Florabox](./img/Verkablung-Florabox.png)
      
- Florachi
    - Komponenten:
          - ESP32
          - OLED-Display (SSD1306, 0.96")
          - KY-006 Passive Buzzzer
          - HC-SR04 Ultra-schallsensor
    - Verkablung:

Software Setup:
- Intialization
    - Open/Download VS Code
    - Install Add on: PlatformIO IDE
- Florabox Arduino (NO WiFi)
    - Go To Projects & Configurations
    - Create New Project (Name: Florabox-Arduino, Board: Arduino Uno, Framework: Arduino)
    - Replace main.cpp and platformio.ini with files from Github
    - Build, Upload and Monitor
- Florabox Rasberry Pi (WiFi)
- Florachi ESP32
