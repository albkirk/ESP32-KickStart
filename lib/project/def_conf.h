// -- PLATFORM Selection --
// #define ESP32                                        // already defined elsewhere...
#undef ESP8266                                          // To make sure it is not used somewhere... 

// -- HARWARE & SOFTWARE Version --
#define BRANDName           "AlBros_Team"               // Hardware brand name
#define MODELName           "GenBox_32"                 // Hardware model name
#define SWVer               "12.15"                     // Major.Minor Software version (use String 01.00 - 99.99 format !)

// -- GPIO to Function Assignment --
#define LED_ESP             13                          // ESP32=22, T-Call=13, -1 means NOT used!
#define BUZZER              -1                          // (Active) Buzzer pin. Suggest to use pin 0. -1 means NOT used!
#define Ext1WakeUP          14                          // External Wake Up pin. (connected to GND, with Parallel Cap).  -1 means NOT used!
#define Def_Config          18                          // Return to Default configuration. -1 means NOT used! 

// -- Power Source & Battery Level --
bool BattPowered =          true;                       // Is the device battery powered?
#define Batt_L_Thrs         26                          // Battery level threshold [0%-100%] (before slepping forever).
#define Using_ADC           true                        // will this device use the ADC? (if not, ES8266 will measure the internal voltage)

// -- I2C PIN Definition --
#define SDAPIN              21                          // I2C SDA PIN. T-Call=21, -1 means NO SDA used!
#define SCKPIN              22                          // I2C SCK PIN. T-Call=22, -1 means NO SCK used!

// -- DHT Definition --
#define DHTTYPE              2                          // use 1 for "DHT11", 2 for "DHT22", or 3 for "AM2320" to select the DHT Model
#define DHTPIN              -1                          // GPIO connected to DHT Data PIN. -1 means NO DHT used!

// -- MODEM Definition --
// Module selection
//#define TINY_GSM_MODEM_SIM7020                        // SIMCOM 7020 module. 
#define TINY_GSM_MODEM_SIM800                           // SIMCOM 800 module. TTGo T-Call module uses this chip
#define IP5306                                          // Power Management chip. TTGo T-Call module uses this.
// Modem PINs
#define MODEM_PWKEY          4                          // T-Call=4, SIM7020E=14, -1 means NOT used!
#define MODEM_RST            5                          // T-Call=5, -1 means NOT used!
#define MODEM_POWER_ON      23                          // T-Call=23, -1 means NOT used!
#define MODEM_RX            26                          // T-Call=26, SIM7020E=4, -1 means NOT used!
#define MODEM_TX            27                          // T-Call=27, SIM7020E=5, -1 means NOT used!
#define GPS_RX              32                          // T-Call=32, ESP8266=14, -1 means NOT used!
#define GPS_TX              33                          // T-Call=33, ESP8266=12, -1 means NOT used!
#define GPS_SW              12                          // Pin for the MOFFET based GPS power switch. -1 means NOT used!


void config_defaults() {
    Serial.println("Setting config Default values");

    strcpy(config.DeviceName, "ESP_Generic");             // Device Name
    strcpy(config.Location, "MainRoom");                  // Device Location
    strcpy(config.ClientID, "001001");                    // Client ID (used on MQTT)
    config.ONTime = 10;                                   // 0-255 seconds (Byte range)
    config.SLEEPTime = 0;                                 // 0-255 minutes (Byte range)
    config.DEEPSLEEP = true;                              // 0 - Disabled, 1 - Enabled
    config.LED = true;                                    // 0 - OFF, 1 - ON
    config.TELNET = true;                                 // 0 - Disabled, 1 - Enabled
    config.OTA = true;                                    // 0 - Disabled, 1 - Enabled
    config.WEB = true;                                    // 0 - Disabled, 1 - Enabled
    config.Remote_Allow = true;                           // 0 - Not Allow, 1 - Allow remote operation
    config.STAMode = true;                                // 0 - AP or AP+STA Mode, 1 - Station only Mode
    config.APMode = true;                                 // 0 - AP Mode Disabled, 1 - AP Mode Enabled
    strcpy(config.SSID, "WiFiCasaN");                     // Wireless LAN SSID (STA mode)
    strcpy(config.WiFiKey, "12345678");                   // Wireless LAN Key (STA mode)
    config.DHCP = true;                                   // 0 - Static IP, 1 - DHCP
    config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 1; config.IP[3] = 10;
    config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
    config.Gateway[0] = 192; config.Gateway[1] = 168; config.Gateway[2] = 1; config.Gateway[3] = 1;
    config.DNS_IP[0] = 192; config.DNS_IP[1] = 168; config.DNS_IP[2] = 1; config.DNS_IP[3] = 93;
    strcpy(config.NTPServerName, "pt.pool.ntp.org");      // NTP Server
    config.Update_Time_Via_NTP_Every = 1200;              // Time in minutes to re-sync the clock
    config.TimeZone = 0;                                  // -12 to 13. See Page_NTPSettings.h why using -120 to 130 on the code.
    config.isDayLightSaving = 1;                          // 0 - Disabled, 1 - Enabled
    strcpy(config.MQTT_Server, "iothubna.hopto.org");     // MQTT Broker Server (URL or IP)
    config.MQTT_Port = 1883;                              // MQTT Broker TCP port
    config.MQTT_Secure = false;                           // 0 - Unsecure, 1 - TLS v1.2 Secured!!
    strcpy(config.MQTT_User, "admin");                    // MQTT Broker username
    strcpy(config.MQTT_Password, "admin");                // MQTT Broker password
    strcpy(config.UPDATE_Server, "iothubna.hopto.org");   // UPDATE Server (URL or IP)
    config.UPDATE_Port = 1880;                            // UPDATE Server TCP port
    strcpy(config.UPDATE_User, "user");                   // UPDATE Server username
    strcpy(config.UPDATE_Password, "1q2w3e4r");           // UPDATE Server password
    strcpy(config.SIMCardPIN, "1234");                    // SIM card PIN
    strcpy(config.APN, "internet.vodafone.pt");           // Modem (GPRS/NB-IoT) APN name 
    strcpy(config.MODEM_User, "none");                    // APN username
    strcpy(config.MODEM_Password, "none");                // APN password
    strcpy(config.WEB_User, "admin");                     // WEB Server username
    strcpy(config.WEB_Password, "admin");                 // WEB Server password
    config.Temp_Corr = 0.0;                               // Sensor Temperature Correction Factor, typically due to electronic self heat.
    config.LDO_Corr = 0.25;                               // Battery Voltage [volt] corrective Factor due to LDO/Diode voltage drop
    config.HW_Module = true;                              // Is HW module plugged (ex.: GPS hardware)used / connected?
    config.HASSIO_CFG = false;                            // Is HASSIO configured? If not, it should perform the discovery.
    config.DEBUG = true;                                  // 0 - No serial msgs, 1 - Debug msg sent to serial interface
    config.SW_Upgraded = false;                           // Is SW Upgrade completed? If not, clean the house and Update status.
    config.SWITCH_Default = false;                        // 0 - OFF, 1 - ON - Default SWITCH Status 
    config.UPPER_LEVEL = 70;                              // level where blinds should stay when pressing double UP
    config.LOWER_LEVEL = 10;                              // level where blinds should stay when pressing double DN
    config.MIN_TRAVEL = 0;                                // Time reference for fully DN position
    config.MAX_TRAVEL = 17000;                            // Time required to go from fully DN to fully UP
    config.Voltage_Multiplier = 326341.66;                // Unit: us/V
    config.Current_Multiplier = 21182.61;                 // Unit: us/A
    config.Power_Multiplier = 792008.54;                  // Unit: us/W
}
