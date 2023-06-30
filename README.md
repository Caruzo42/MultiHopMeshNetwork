# LoRa Multi-Hop Netzwerk Projekt

Dieses Repository beinhaltet ein Projekt, das im Rahmen der Veranstaltung "MD&MA" im Sommersemester 2023 durchgeführt wurde. In diesem Projekt wurde ein Multi-Hop-Netzwerk mit der LoRa-Funktechnologie und ESP32-Mikrocontrollern (Heltec Wifi LoRa 32 v2) aufgebaut. Die Kommunikation erfolgt über eine eigene, modifizierte Implementierung des MQTT-Protokolls (genauer definiert in Interface.md). Beispielhaft sind sowohl Code für das Gateway, das mit dem Backend des Projektes interagieren sollte, sowie Code für Measurements enthalten.

Das Projekt verwendet Platform.io und die folgenden Bibliotheken aus dem Platform.io Library Manager:
- thingpulse/ESP8266 and ESP32 OLED driver for SSD1306 displays@^4.4.0
- mikem/RadioHead@^1.120
- ricmoo/QRCode@^0.0.1

Das Platform.io Projekt befindet sich im Unterordner MultiHopNetzwerk.

Das Projekt definiert zwei Betriebsumgebungen: 'Gateway' und 'Node'. Vor dem Aufspielen des Codes auf einen Mikrocontroller muss eine dieser Umgebungen ausgewählt werden. 

## Code-Dateien

Die Code-Dateien des Projekts sind wie folgt strukturiert:
Datei | Beschreibung
--- | ---
`config.h` | Beinhaltet die grundlegenden Konfigurationseinstellungen des Projekts.
`display.h`, `display.cpp` | Verantwortlich für die Implementierung der Displayfunktionen.
`gateway.h`, `gateway.cpp` | Enthalten die `setup()` und `loop()` Methoden, welche auf dem Gateway-ESP32 ausgeführt werden.
`measurements.h`, `measurements.cpp` | Enthalten beispielhaften Code für die Durchführung und Verwaltung von Messungen.
`network.h`, `network.cpp` | Stellen den Code zur Interaktion mit der Radiohead-Bibliothek bereit und unterstützen den Aufbau des Netzwerks.
`node.h`, `node.cpp` | Enthalten die `setup()` und `loop()` Methoden, die auf der Node-ESP32 ausgeführt werden.
`protocol.h`, `protocol.cpp` | Beinhalten Methoden zur Serialisierung und zum Parsen von Nachrichten gemäß dem definierten Protokoll.
`protocol_common.h`, `protocol_common.cpp` | Definieren die Structs Message, FixedHeader und UpdateBlock und stellen Factory-Methoden für diese Structs bereit.
`variable_headers.h`, `variable_headers.cpp` | Definieren die Structs für die verschiedenen Arten von variablen Headern im benutzerdefinierten MQTT-Protokoll.
