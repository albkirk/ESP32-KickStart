# ESP32 Project "Kick Start" Template


This is a personal project to create a starting point for all my ESP projects.
It is written in C++ under PlatformIO IDE (integrated on ATOM platform).
I'm coding my own variant of this popular project, with some inspiration and lessons (code Snippets) from some well know projects like:

- ESPURNA: [GitHub Espurna Page](https://github.com/SensorsIot/Espurna-Framework)
- TASMOTA: [GitHub Tasmota Page](https://github.com/arendst/Sonoff-Tasmota)

## Supported Features:
	1. Project should run locally, even when LAN (WiFi) newtork is down (it seems obvious, but ...)
	2. Remote upgrade Over-the-Air (OTA) or/and HTTP Update.
	3. Locally store configuration (ex.: DeviceName, Location, ...) 
	4. MQTT Publish/Subscribe support
        	4.1.  ALl data is sent under "/<clientID>/<location>/<device name>/*telemetry*/<topic name>" in string format
        	4.2.  ALl data received sent under "/<clientID>/<location>/<device name>/*configure*/<topic name>" in string format
	5. User commands feedback by flashing the "internal" LED and/or adittional active buzzer.
	6. OTA, TELNET, LED, RESET, REBOOT, STORE and DEEPSLEEP functionalities can be remotely enabled/disabled/triggered via defined "true"/"false" MQTT commands.
	7. WiFi and Battery status sent via MQTT
	8. DeepSleep support with configurable On-Time(+ extended time) and Sleep-Time
	9. Remote "Debug" using Telnet (enabled via MQTT)
	10. Long operational live (it will be running 365 days a year, so, it recovers from Wifi or MQTT loss)
	11. Date / Time with NTP sync
	12. WiFi "Air" sniffing for APs, Registered Stations and "Unregistered" Stations

## Future features!!
	1. Web Page for "minimum" configuration (it kinda works...)
	2. Power consumption optimization
	3. Secure communication (https or other methods)
   
