# Florachi – HCI Projekt

Florachi ist ein virtueller Pflanzenbegleiter, der den Zustand einer realen Pflanze über einen Avatar emotional darstellt.  
Die **Florabox** misst Umweltwerte (Temperatur, Licht, Wasser) und berechnet daraus einen Health-Wert.  
Ein **Raspberry Pi** stellt diesen Wert per HTTP bereit.  
**Florachi (ESP32)** ruft den Wert ab und zeigt Emotionen auf einem OLED-Display an.

---

## Hinweis zur Hardware
Da kein Arduino Uno WiFi verfügbar war, wird ein **Arduino Uno R3 + Raspberry Pi 3 Model B** eingesetzt.  
Falls ein Arduino Uno WiFi oder ein ESP32 für die Florabox verfügbar ist, kann der Aufbau vereinfacht werden (Raspberry Pi entfällt).

---
# 1. Systemübersicht

Die Architektur besteht aus drei Modulen, die über Serial und HTTP miteinander kommunizieren:
![Systemarchitektur](./img/systemarchitektur.png)

## Module

### Florabox (Arduino Uno R3)
- Sensoren für **Feuchtigkeit**, **Licht**, **Temperatur**
- Berechnung des **Health-Wertes**
- Übertragung via **USB / Serial (9600 Baud)**

### Lokaler Server (Raspberry Pi 3/4)
- Liest serielle Daten vom Arduino
- Stellt den Health-Wert über eine **HTTP-API (Flask)** bereit
- Brücke zwischen kabelgebundener Florabox und WLAN-Client

### Florachi (ESP32)
- Ruft Health-Werte via **WLAN / HTTP GET** ab
- Zeigt Emotionen über OLED-Display
- Spielt Sounds (Buzzer)
- Aktivierung durch Gestenerkennung (Ultraschallsensor)

## Kommunikationsfluss

1. **Arduino → Raspberry Pi**: Serial (9600 Baud), Übertragung des Health-Werts  
2. **Raspberry Pi → ESP32**: HTTP-API im lokalen Netzwerk  
3. **ESP32 → Nutzer**: Emotionen & Sounds basierend auf dem Health-Wert
---
# 2. Hardware

## Florabox – Komponenten

| **Komponente** | **Verwendung** | **Anzahl** |
|----------------|----------------|------------|
| Arduino Uno R3 | Steuereinheit, Sensorverarbeitung, Berechnung des Health-Wertes | 1 |
| Raspberry Pi 4 Model B | Serielles Auslesen, Bereitstellen der HTTP-API | 1 |
| KY-013 Temperatursensor | Misst Temperatur | 1 |
| KY-018 Lichtsensor | Misst Lichtintensität | 1 |
| Wassersensor-Modul | Misst Feuchtigkeit / Wasserstand | 1 |
| LCD1602 Display | Anzeige der Messwerte und Health | 1 |
| KY-004 Button | Menü-/Displaysteuerung | 1 |
| Breadboard | Aufbau & Verkabelung | 1 |
| Jumperkabel | elektrische Verbindung aller Module | ca. 25 |
| Widerstände (z. B. 10kΩ) | Signalanpassung / Schutz | 2 |

### Verkabelung
- Sensoren → 5V / GND / analoger Pin  
- LCD1602 → RS, E, D4–D7 an digitale Pins  
- Button → digitaler Pin mit Pulldown/-up  
- Arduino → Raspberry Pi über USB  
- Referenzdiagramm:

![Verkabelung Florabox](./img/Verkablung-Florabox.png)

---

## Florachi – Komponenten

| **Komponente** | **Verwendung** | **Anzahl** |
|----------------|----------------|------------|
| ESP32 (Devkit V1) | Hauptcontroller, WLAN, HTTP-Abfragen | 1 |
| OLED-Display (0.96", SSD1306) | Darstellung der Emotionen | 1 |
| KY-006 Passiv-Buzzer | akustische Signale | 1 |
| HC-SR04 Ultraschallsensor | Gesten-/Annäherungserkennung | 1 |
| 1→3-Verzweigung (3.3V) | Aufteilung der Spannungsversorgung | 1 |
| 1→3-Verzweigung (GND) | Aufteilung der Masseleitung | 1 |
| Female–Female Jumperkabel | Signalverbindungen (SCL, SDA, Trig, Echo, Buzzer) | 5 |

### Verkabelung
- OLED → SDA/SCL  
- HC-SR04 → Trig/Echo  
- Buzzer → PWM-Pin  
- ESP32 → 3.3V und GND (über 1→3-Verteilung)

![Verkabelung Florachi](./img/Verkablung-Florachi.png)

---

# 3. Software Setup

## Voraussetzungen
- Visual Studio Code  
- PlatformIO IDE Extension  

---

## Florabox – Arduino Uno (ohne WiFi)

1. In PlatformIO ein neues Projekt erstellen  
   - Board: **Arduino Uno**  
   - Framework: **Arduino**
2. Im Projektordner die Dateien aus dem Repository ersetzen:  
   - `main.cpp`  
   - `platformio.ini`
3. Projekt **Build → Upload**  
4. Über **Monitor** prüfen, ob Sensordaten seriell ausgegeben werden

---

## Raspberry Pi – Server

1. Arduino per USB mit dem Raspberry Pi verbinden  
2. Projektordner für den Server auf den Pi kopieren  
3. Abhängigkeiten installieren:  
   ```bash
   pip install flask pyserial
4. Server starten  
   ```bash  
   python3 server.py  
5. Health-Wert ist erreichbar unter  
   ```bash  
   http://<raspberry-pi-ip>/  
---

## Florachi – ESP32
### Setup

1. In PlatformIO ein neues Projekt erstellen  
   - Board: **ESP32 Devkit V1**  
   - Framework: **Arduino**

2. Folgende Dateien aus dem Repository in das Projekt kopieren:  
   - `main.cpp` – Hauptlogik (HTTP-Abruf, Sensorsteuerung, Ablauf)  
   - `eyes.cpp` – Grafiken / Funktionen für Emotionen  
   - `eyes.h` – Header für die Emotionen-Logik  
   - `platformio.ini` – Board- und Libraries-Konfiguration

3. In `main.cpp` konfigurieren:  
   - WLAN-SSID  
   - WLAN-Passwort  
   - IP-Adresse des Raspberry Pi (HTTP-Server)

### Build und Upload

1. Projekt kompilieren  
2. ESP32 per USB anschließen  
3. Upload durchführen  
4. Serial Monitor öffnen und prüfen:  
   - WLAN-Verbindung aktiv  
   - HTTP-GET erfolgreich  
   - Health-Werte werden empfangen  
   - OLED zeigt Emotionen korrekt an
