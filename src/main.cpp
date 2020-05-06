/*
 * * Main File to trigger all C code using "arduino" style.
 * * It contains all necessary function calls on setup and loop functions
 * * HOW TO USE THIS TEMPLATE:
 * * -- Adjust the parameter below to your project.
 * *    Parameters on struct "config" will be store on memory.
 * *    Don't forget to customize the Mem read and write actions on "lib/project/custostore.h"
 * * -- Use the "// **** Normal code ..." zones to add you own definition, functions, setup and loop code
 * * -- You can also add you own MQTT actions on "lib/project/customqtt.h"
 * * -- Suggest to use "lib/project/" to add your own h files
 */
#include <Arduino.h>
#include <rom/rtc.h>
#include <WiFi.h>

// HARWARE & SOFTWARE Version
#define BRANDName "AlBros_Team"                         // Hardware brand name
#define MODELName "GenBox_A"                            // Hardware model name
#define SWVer "10.13"                                   // Major.Minor Software version (use String 01.00 - 99.99 format !)

// Power Source & Battery Level
bool BattPowered = true;                                // Is the device battery powered?
#define Batt_L_Thrs 15                                  // Battery level threshold [0%-100%] (before slepping forever).

// GPIO to Function Assignment
#define Using_ADC true                                  // will this device use the ADC? (if not it will measure the internal voltage)
#define LED_esp 22                                      // ESP Led is connected to GPIO 2. -1 means NOT used!
#define DHTTYPE 2                                       // use 1 for "DHT11", 2 for "DHT22", or 3 for "AM2320" to select the DHT Model
#define DHTPIN -1                                       // GPIO connected to DHT Data PIN. -1 means NO DHT used!
#define SDAPIN -1                                       // GPIO connected to (AM) I2C SDA PIN. -1 means NO SDA used!
#define SCKPIN -1                                       // GPIO connected to (AM) I2C SCK PIN. -1 means NO SCK used!
#define BUZZER -1                                       // (Active) Buzzer pin. Suggest to use pin 0.  -1 means NOT used!


struct __attribute__((__packed__)) strConfig {
  char DeviceName[16];
  char Location[16];
  char ClientID[8];
  byte ONTime;
  byte SLEEPTime;
  bool DEEPSLEEP;
  bool LED;
  bool TELNET;
  bool OTA;
  bool WEB;
  bool Remote_Allow;
  bool STAMode;
  char ssid[32];
  char WiFiKey[32];
  bool DHCP;
  byte IP[4];
  byte Netmask[4];
  byte Gateway[4];
  char NTPServerName[128];
  long TimeZone;
  unsigned long Update_Time_Via_NTP_Every;
  bool isDayLightSaving;
  char MQTT_Server[128];
  long MQTT_Port;
  char MQTT_User[16];
  char MQTT_Password[16];
  char UPDATE_Server[128];
  long UPDATE_Port;
  char UPDATE_User[16];
  char UPDATE_Password[16];
  bool SWITCH_Default;
  float Temp_Corr;
  float LDO_Corr;
} config;


void config_defaults() {
    Serial.println("Setting config Default values");

    strcpy(config.DeviceName, "ESP_Generic");             // Device Name
    strcpy(config.Location, "MainRoom");                  // Device Location
    strcpy(config.ClientID, "001001");                    // Client ID (used on MQTT)
    config.ONTime = 60;                                   // 0-255 seconds (Byte range)
    config.SLEEPTime = 0;                                 // 0-255 minutes (Byte range)
    config.DEEPSLEEP = false;                             // 0 - Disabled, 1 - Enabled
    config.LED = true;                                    // 0 - OFF, 1 - ON
    config.TELNET = false;                                // 0 - Disabled, 1 - Enabled
    config.OTA = true;                                    // 0 - Disabled, 1 - Enabled
    config.WEB = false;                                   // 0 - Disabled, 1 - Enabled
    config.Remote_Allow = true;                           // 0 - Not Allow, 1 - Allow remote operation
    config.STAMode = true;                                // 0 - AP or AP+STA Mode, 1 - Station only Mode
    strcpy(config.ssid, "WiFiCasaN");                     // Wireless LAN SSID (STA mode)
    strcpy(config.WiFiKey, "12345678");                   // Wireless LAN Key (STA mode)
    config.DHCP = true;                                   // 0 - Static IP, 1 - DHCP
    config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 1; config.IP[3] = 10;
    config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
    config.Gateway[0] = 192; config.Gateway[1] = 168; config.Gateway[2] = 1; config.Gateway[3] = 254;
    strcpy(config.NTPServerName, "pt.pool.ntp.org");      // NTP Server
    config.Update_Time_Via_NTP_Every = 1200;              // Time in minutes to re-sync the clock
    config.TimeZone = 0;                                  // -12 to 13. See Page_NTPSettings.h why using -120 to 130 on the code.
    config.isDayLightSaving = 1;                          // 0 - Disabled, 1 - Enabled
    strcpy(config.MQTT_Server, "iothubna.hopto.org");     // MQTT Broker Server (URL or IP)
    config.MQTT_Port = 1883;                              // MQTT Broker TCP port
    strcpy(config.MQTT_User, "admin");                    // MQTT Broker username
    strcpy(config.MQTT_Password, "admin");                // MQTT Broker password
    strcpy(config.UPDATE_Server, "iothubna.hopto.org");   // UPDATE Server (URL or IP)
    config.UPDATE_Port = 1880;                            // UPDATE Server TCP port
    strcpy(config.UPDATE_User, "user");                   // UPDATE Server username
    strcpy(config.UPDATE_Password, "1q2w3e4r");           // UPDATE Server password
    config.SWITCH_Default = false;                        // 0 - OFF, 1 - ON - Default SWITCH Status 
    config.Temp_Corr = 0.0;                               // Sensor Temperature Correction Factor, typically due to electronic self heat.
    config.LDO_Corr = 0.6;                                // Battery Voltage [volt] corrective Factor due to LDO/Diode voltage drop
}


// Libraries to INCLUDE
#include <storage.h>
#include <esp32hw.h>
#include <mywifi.h>
#include <telnet.h>
#include <mqtt.h>
#include <ntp.h>
#include <ota.h>
//#include <web.h>
#include <global.h>
#include <project.h>
#include <mqttactions.h>                                  // Added later because functions from project are called here.


void setup() {
// Starting with WiFi interface shutdown in order to save energy
    WiFi.mode(WIFI_MODE_NULL);

  // Start Serial interface
  //Serial.begin(74880);                  // This odd baud speed will show ESP8266 boot diagnostics too.
  Serial.begin(115200);                   // For faster communication use 115200

  Serial.println(" ");
  Serial.println("Hello World!");
  Serial.println("My ID is " + ChipID + " and I'm running version " + SWVer);
      Serial.println("Reset reason: " + ESPWakeUpReason());

  // Start Storage service and read stored configuration
      storage_setup();

  // Start ESP specific features, such as: Serial Number, ESP_LED, internal ADC, ...
      hw_setup();

  //  Project HW initialization, such as: GPIO config, Sensors, Actuators, ...  
      project_hw();

  // Start WiFi service (Station or/and as Access Point)
      wifi_setup();

  // Start TELNET service
      if (config.TELNET) telnet_setup();

 // Start MQTT service
      mqtt_setup();

  // Start NTP service
      ntp_setup();

  // Start OTA service
      if (config.OTA) ota_setup();
      ota_http_upg();

  // Start ESP Web Service
      //if (config.WEB) web_setup();


  // **** Project SETUP Sketch code here...
      project_setup();


  // Last bit of code before leave setup
      ONTime_Offset = millis()/1000 + 0.1;  //  100ms after finishing the SETUP function it starts the "ONTime" countdown.
                                            //  it should be good enough to receive the MQTT "ExtendONTime" msg
} // end of setup()


void loop() {
  // allow background process to run.
      yield();

  // Hardware handling, namely the ESP_LED
      hw_loop();

  // WiFi handling
      wifi_loop();

  // TELNET handling
      if (config.TELNET) telnet_loop();

  // MQTT handling
      mqtt_loop();

  // NTP handling
      ntp_loop();

  // OTA request handling
      if (config.OTA) ota_loop();

  // ESP Web Server requests handling
      //if (config.WEB) web_loop();

  // Global loops handling
      deepsleep_loop();
      if (BattPowered && ((millis() - 2500) % 60000 < 15)) Batt_OK_check();    // If Batt LOW, it will DeepSleep forever!

  // **** Project LOOP Sketch code here ...
      project_loop();


}  // end of loop()
