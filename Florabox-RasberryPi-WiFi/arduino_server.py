import serial
import threading
import time
from flask import Flask, Response

app = Flask(__name__)

ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
time.sleep(2)

last_health = "Health: 0.0"   # Globaler Zustand für Health

def read_serial():
    global last_health
    while True:
        if ser.in_waiting > 0:   # Korrigiert: Bedingung für ser.in_waiting
            line = ser.readline().decode('utf-8').strip()
            print("Empfangen:", line)
            if line.startswith("Health:"):
                last_health = line   # Health-Status aktualisieren

@app.route('/')
def index():
    return Response(last_health, mimetype='text/plain')  # Rückgabe des Health-Status

# Starte den Serial-Thread
threading.Thread(target=read_serial, daemon=True).start()

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)   # Flask läuft auf allen IP-Adressen
