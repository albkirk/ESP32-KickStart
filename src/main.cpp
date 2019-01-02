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
#define SWVer "10.03"                                   // Major.Minor Software version (use String 01.00 - 99.99 format !)

// Battery & ESP Voltage
#define BattPowered true                                // Is the device battery powered?
#define LDO_Corr float(0.4)                             // Battery Voltage [volt] corrective Factor due to LDO/Diode voltage drop
#define Batt_L_Thrs 40                                  // Battery level threshold [0%-100%] (before slepping forever).

// GPIO to Function Assignment
#define Using_ADC false                                 // will this device use the ADC? (if not it will measure the interval voltage)
#define LED_esp 22                                      // ESP Led is connected to GPIO x. -1 means NOT used!
#define DHTPIN -1                                       // GPIO Connected to DHT22 Data PIN. -1 means NO DHT used!
#define BUZZER -1                                       // (Active) Buzzer pin. Suggest to use pin 0.  -1 means NOT used!


struct strConfig {
  String DeviceName;
  String Location;
  String ClientID;
  byte ONTime;
  byte SLEEPTime;
  boolean DEEPSLEEP;
  boolean LED;
  boolean TELNET;
  boolean OTA;
  boolean WEB;
  boolean Remote_Allow;
  boolean STAMode;
  String ssid;
  String WiFiKey;
  boolean dhcp;
  byte IP[4];
  byte Netmask[4];
  byte Gateway[4];
  String NTPServerName;
  long TimeZone;
  long Update_Time_Via_NTP_Every;
  boolean isDayLightSaving;
  String MQTT_Server;
  long MQTT_Port;
  String MQTT_User;
  String MQTT_Password;
  String UPDATE_Server;
  long UPDATE_Port;
  String UPDATE_User;
  String UPDATE_Password;
  long Temp_Corr;
  byte Volume;
} config;


void config_defaults() {
    Serial.println("Setting config Default values");

    config.DeviceName = String("ESP_Generic");            // Device Name
    config.Location = String("MainRoom");                 // Device Location
    config.ClientID = String("001001");                   // Client ID (used on MQTT)
    config.ONTime = 60;                                   // 0-255 seconds (Byte range)
    config.SLEEPTime = 0;                                 // 0-255 minutes (Byte range)
    config.DEEPSLEEP = false;                             // 0 - Disabled, 1 - Enabled
    config.LED = true;                                    // 0 - OFF, 1 - ON
    config.TELNET = true;                                 // 0 - Disabled, 1 - Enabled
    config.OTA = true;                                    // 0 - Disabled, 1 - Enabled
    config.WEB = false;                                   // 0 - Disabled, 1 - Enabled
    config.Remote_Allow = true;                           // 0 - Not Allow, 1 - Allow remote operation
    config.STAMode = true;                                // 0 - AP or AP+STA Mode, 1 - Station only Mode
    //config.ssid = String("ALU_MEO_IPTV");                 // Wireless LAN SSID (STA mode)
    //config.WiFiKey = String("ALUME0IPTV");                  // Wireless LAN Key (STA mode)
    config.ssid = String("ThomsonCasaN");                 // Wireless LAN SSID (STA mode)
    config.WiFiKey = String("12345678");                  // Wireless LAN Key (STA mode)
    config.dhcp = true;                                   // 0 - Static IP, 1 - DHCP
    config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 1; config.IP[3] = 10;
    config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
    config.Gateway[0] = 192; config.Gateway[1] = 168; config.Gateway[2] = 1; config.Gateway[3] = 254;
    config.NTPServerName = String("pt.pool.ntp.org");     // NTP Server
    config.Update_Time_Via_NTP_Every = 1200;              // Time in minutes to re-sync the clock
    config.TimeZone = 0;                                  // -12 to 13. See Page_NTPSettings.h why using -120 to 130 on the code.
    config.isDayLightSaving = 1;                          // 0 - Disabled, 1 - Enabled
    config.MQTT_Server = String("iothubna.hopto.org");    // MQTT Broker Server (URL or IP)
    config.MQTT_Port = 1883;                              // MQTT Broker TCP port
    config.MQTT_User = String("admin");                   // MQTT Broker username
    config.MQTT_Password = String("admin");               // MQTT Broker password
    config.UPDATE_Server = String("iothubna.hopto.org");  // UPDATE Server (URL or IP)
    config.UPDATE_Port = 1880;                            // UPDATE Server TCP port
    config.UPDATE_User = String("user");                  // UPDATE Server username
    config.UPDATE_Password = String("1q2w3e4r");          // UPDATE Server password
    config.Temp_Corr = 0;     // Sensor Temperature Correction Factor, typically due to electronic self heat.
    config.Volume = 100;      // Speaker volume [0-100%].

}


#include <storage.h>
#include <esp32hw.h>
#include <mywifi.h>
#include <telnet.h>
#include <ntp.h>
//#include <web.h>
#include <ota.h>
#include <mqtt.h>
#include <global.h>


// **** Normal code definition here ...
#define rotate 1
#include <tft.h>
#include <buttons.h>
#include <sounds.h>
#include <dacplayer.h>
#include <menu.h>

// **** Normal code functions here ...


void setup() {
// Start Serial interface
  //Serial.begin(74880);                      //This odd baud speed will show ESP8266 boot diagnostics too.
  Serial.begin(115200);                   // For faster communication use 115200

  Serial.println(" ");
  Serial.println("Hello World!");
  Serial.println("My ID is " + ChipID + " and I'm running version " + SWVer);
  Serial.println("Reset reason: " + String(ESPWakeUpReason()));

  // Output GPIOs

  // Input GPIOs

  // Start Hardware services, like: ESP_LED. DHT, internal ADC,...
      hw_setup();

  // Start Storage service and read stored configuration
      //config_defaults();
      storage_setup();

  // Start WiFi service (Station or/and as Access Point)
      wifi_setup();

  // Start NTP service
      ntp_setup();

  // Start TELNET service
      if (config.TELNET) telnet_setup();

  // Start OTA service
      if (config.OTA) ota_setup();

  // Start ESP Web Service
      //if (config.WEB) web_setup();

  // Start MQTT service
      mqtt_setup();

  //  LOW Battery check
      //LOW_Batt_check();               // Must be execute after mqtt_setup. If LOW Batt, it will DeepSleep forever!


  // **** Normal SETUP Sketch code here...
  // Start TFT device
      tft_setup();

  // Start Buttons device
      buttons_setup();

  // Start Player device
      config.Volume = 50;
      player_beep();
      //player_music(startup_sound, 20000);


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

  // NTP handling
      ntp_loop();

  // TELNET handling
      if (config.TELNET) telnet_loop();

  // OTA request handling
      if (config.OTA) ota_loop();

  // ESP Web Server requests handling
      //if (config.WEB) web_loop();

  // MQTT handling
      mqtt_loop();

  // DeepSleep handling
      deepsleep_loop();

  // **** Normal LOOP Skecth code here ...
      menu_loop();

  // Player handling
      player_loop();

}  // end of loop()