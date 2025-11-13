# HCI_Florachi

Projekt Beschreibung:

Florachi wird durch die Florabox ergänzt, die kontinuierlich Umweltparameter erfasst, um den Zustand der Pflanze zu bestimmen. Diese Daten werden an einen Webserver übertragen und an Florachi weitergegeben. Der virtuelle Charakter reagiert darauf, indem er seinen Gesichtsausdruck anpasst. So entsteht ein digitaler Zwilling, der den Zustand der Pflanze emotional visualisiert. Das Konzept knüpft an Forschung zu virtuellen Begleitern und Avataren an. Chen et al. (2011) zeigen, dass virtuelle Charaktere, deren Verhalten auf das Handeln der Nutzenden reagiert, Verantwortungsbewusstsein und Engagement fördern können. In ihrer Studie spiegelten virtuelle Haustiere das Lernverhalten von Kindern wider, wo-durch emotionale Bindung und Selbstwahrnehmung gestärkt wurden. Übertragen auf Florachi bedeutet dies, dass die Reaktion des Avatars auf reale Pflanzendaten die Aufmerksamkeit und Pflegebereitschaft der Nutzenden fördern kann. Zhang und Wu (2024) belegen zudem, dass die visuelle Ausdrucksfähigkeit virtueller Avatare einen positiven Einfluss auf Lernerfolg, emotionale Erfahrung und Nutzerbindung hat. Eine glaubwürdige, ausdrucksstarke Darstellung steigert das Interesse und erleichtert die Identifikation mit dem dargestellten Inhalt. Florachi verbindet diese Ansätze, um den Zustand einer Pflanze über einen virtuellen Avatar emotional erfahrbar zu machen. Durch die Übersetzung von Sensordaten in visuelle Reaktionen entsteht eine intuitive Rückkopplung zwischen Mensch und Pflanze. Ziel ist es, das Bewusstsein für Pflegehandlungen zu stärken und eine nachhaltigere Beziehung zur Umwelt zu fördern.

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
