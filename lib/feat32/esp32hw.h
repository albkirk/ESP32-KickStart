#include <wifi.h>
//#include <BLEDevice.h>
#include <rom/rtc.h>
#include <Preferences.h>
#include <WebServer.h>
#ifdef IP5306
    #include <ip5306.h>
#endif
#include <myulp.h>


uint32_t getChipId();
String HEXtoUpperString(uint32_t hexval, uint hexlen);

//System Parameters
String ChipID=HEXtoUpperString(getChipId(), 6);
#define ESP_SSID String("ESP-" + ChipID)    // SSID to use as Access Point
#define Number_of_measures 5                // Number of value samples (measurements) to calculate average
byte SLEEPTime = config.SLEEPTime;          // Variable to allow temporary change the sleeptime (ex.: = 0)
bool Celular_Connected = false;             // Modem Connection state


// The ESP8266 RTC memory is arranged into blocks of 4 bytes. The access methods read and write 4 bytes at a time,
// so the RTC data structure should be padded to a 4-byte multiple.
struct __attribute__((__packed__)) struct_RTC {
  uint32_t crc32 = 0U;                      // 4 bytes   4 in total
  uint8_t bssid[6];                         // 6 bytes, 10 in total
  uint8_t LastWiFiChannel = 0;              // 1 byte,  11 in total
  uint8_t padding = 0;                      // 1 byte,  12 in total
  unsigned long lastUTCTime = 0UL;          // 4 bytes? 16 in total
} rtcData;

Preferences preferences;                    // Preferences library is wrapper around Non-volatile storage on ESP32 processor.

/*
// ADC to internal voltage
#if Using_ADC == false
    ADC_MODE(ADC_VCC)                       // Get voltage from Internal ADC
#endif
*/
#define Default_ADC_PIN 36

// Initialize the Webserver
WebServer MyWebServer(80);

// initialize WiFi Security
// WiFiSec WiFiSec(CA_CERT_PROG, CLIENT_CERT_PROG, CLIENT_KEY_PROG);
WiFiClient secureclient;                    // Use this for secure connection
WiFiClient unsecuclient;                    // Use this for unsecure connection


// Battery & ESP Voltage
#define Batt_Max float(4.1)                 // Battery Highest voltage.  [v]
#define Batt_Min float(2.8)                 // Battery lowest voltage.   [v]
#define Vcc float(3.3)                      // Theoretical/Typical ESP voltage. [v]
#define VADC_MAX float(3.3)                 // Maximum ADC Voltage input

// Timers for millis used on Sleeping and LED flash
unsigned long ONTime_Offset=0;              // [msec]
unsigned long Extend_time=0;                // [sec]
unsigned long now_millis=0;
unsigned long Pace_millis=3000;
unsigned long LED_millis=300;               // 10 slots available (3000 / 300)
unsigned long BUZZER_millis=100;            // Buzz time (120ms Sound + 120ms  Silent)


// Standard Actuators STATUS
float CALIBRATE = 0;                        // float
float CALIBRATE_Last = 0;                   // float
unsigned int LEVEL = 0;                     // [0-100]
unsigned int LEVEL_Last = 0;                // [0-100]
int POSITION = 0;                           // [-100,+100]
int POSITION_Last = 0;                      // [-100,+100]
bool SWITCH = false;                        // [OFF / ON]
bool SWITCH_Last = false;                   // [OFF / ON]
unsigned long TIMER = 0;                    // [0-7200]  Minutes                 
unsigned long TIMER_Last = 0;               // [0-7200]  Minutes                 
static long TIMER_Current = 0;
unsigned long COUNTER = 0;


// Functions //
uint32_t getChipId() {
  uint8_t chipid[6];
  uint32_t output = 0;
  esp_efuse_mac_get_default(chipid);
  output = ((chipid[3] << 16) & 0xFF0000) + ((chipid[4] << 8) & 0xFF00) + ((chipid[5]) & 0xFF);
  return output;
}

String HEXtoUpperString(uint32_t hexval, uint hexlen) {
    String strgval = String(hexval, HEX);
    String PADZero;
    for (uint i = 0; i < (hexlen - strgval.length()) ; i++) PADZero +="0";
    strgval = PADZero + strgval;
    char buffer[hexlen+1];
    strcpy(buffer, strgval.c_str());
    for (uint i = 0; i < strgval.length() ; i++) {
        if (char(buffer[i]) >= 97 ) buffer[i] = (char)(char(buffer[i] - 32));
    }
    return String(buffer);
}

String CharArray_to_StringHEX(const char *CharArray_value, uint CharArray_length) {
    String strgHEX = "";
    for (size_t i = 0; i < CharArray_length; i++)
    {
        strgHEX +=  String(CharArray_value[i], HEX);
    }
    return strgHEX;
}

uint32_t calculateCRC32( const uint8_t *data, size_t length ) {
  uint32_t crc = 0xffffffff;
  while( length-- ) {
    uint8_t c = *data++;
    for( uint32_t i = 0x80; i > 0; i >>= 1 ) {
      bool bit = crc & 0x80000000;
      if( c & i ) {
        bit = !bit;
      }

      crc <<= 1;
      if( bit ) {
        crc ^= 0x04c11db7;
      }
    }
  }

  return crc;
}

void keep_IP_address() {
if (config.DHCP) {
  if (  WiFi.status() == WL_CONNECTED && IPAddress(config.IP[0], config.IP[1], config.IP[2], config.IP[3]) != WiFi.localIP() ) {
    if (config.DEBUG) {Serial.print("OLD: "); Serial.print(IPAddress(config.IP[0], config.IP[1], config.IP[2], config.IP[3])); Serial.print("\t NEW: "); Serial.println(WiFi.localIP());}
    config.IP[0] = WiFi.localIP()[0]; config.IP[1] = WiFi.localIP()[1]; config.IP[2] = WiFi.localIP()[2]; config.IP[3] = WiFi.localIP()[3];
    config.Netmask[0] = WiFi.subnetMask()[0]; config.Netmask[1] = WiFi.subnetMask()[1]; config.Netmask[2] = WiFi.subnetMask()[2]; config.Netmask[3] = WiFi.subnetMask()[3];
    config.Gateway[0] = WiFi.gatewayIP()[0]; config.Gateway[1] = WiFi.gatewayIP()[1]; config.Gateway[2] = WiFi.gatewayIP()[2]; config.Gateway[3] = WiFi.gatewayIP()[3];
    config.DNS_IP[0] = WiFi.dnsIP()[0]; config.DNS_IP[1] = WiFi.dnsIP()[1]; config.DNS_IP[2] = WiFi.dnsIP()[2]; config.DNS_IP[3] = WiFi.dnsIP()[3];
    if (config.DEBUG) Serial.println("Storing new Static IP address for quick boot");
    storage_write();
    }
}
}

void myconfigTime(const char* tz, const char* server1, const char* server2, const char* server3) {
    configTzTime(tz, server1, server2, server3);
}

void wifi_disconnect() {
    WiFi.mode(WIFI_MODE_NULL);
}

void wifi_hostname() {
    String host_name = String(config.DeviceName + String("-") + config.Location);
    WiFi.setHostname(host_name.c_str());
}

uint8_t wifi_waitForConnectResult(unsigned long timeout) {
    return WiFi.waitForConnectResult();
}

bool RTC_read()  {return false;}
bool RTC_write() {return true;}
bool RTC_reset() {return true;}


/*
// Read RTC memory (where the Wifi data is stored)
bool RTC_read() {
    if (ESP.rtcUserMemoryRead(0, (uint32_t*) &rtcData, sizeof(rtcData))) {
        // Calculate the CRC of what we just read from RTC memory, but skip the first 4 bytes as that's the checksum itself.
        uint32_t crc = calculateCRC32( ((uint8_t*)&rtcData) + 4, sizeof( rtcData ) - 4 );
        if( crc == rtcData.crc32 ) {
            if (config.DEBUG) {
             Serial.print("Read  crc: " + String(rtcData.crc32) + "\t");
             Serial.print("Read  BSSID: " + CharArray_to_StringHEX((const char*)rtcData.bssid, sizeof(rtcData.bssid)) + "\t");
             Serial.print("Read  LastWiFiChannel: " + String(rtcData.LastWiFiChannel) + "\t");
             Serial.println("Read  Last Date: " + String(rtcData.lastUTCTime));
            }
             return true;
        }
    }
    return false;
}

bool RTC_write() {
// Update rtcData structure
    rtcData.LastWiFiChannel = WiFi.channel();
    memcpy( rtcData.bssid, WiFi.BSSID(), 6 ); // Copy 6 bytes of BSSID (AP's MAC address)
    rtcData.crc32 = calculateCRC32( ((uint8_t*)&rtcData) + 4, sizeof( rtcData ) - 4 );
    //rtcData.lastUTCTime = curUnixTime();
    if (config.DEBUG) {
    Serial.print("Write crc: " + String(rtcData.crc32) + "\t");
    Serial.print("Write BSSID: " + CharArray_to_StringHEX((const char*)rtcData.bssid, sizeof(rtcData.bssid)) + "\t");
    Serial.print("Write LastWiFiChannel: " + String(rtcData.LastWiFiChannel) + "\t");
    Serial.println("Write Last Date: " + String(rtcData.lastUTCTime));
    }

// Write rtcData back to RTC memory
    if (ESP.rtcUserMemoryWrite(0, (uint32_t*) &rtcData, sizeof(rtcData))) return true;
    else return false;
}


bool RTC_reset() {
// Update rtcData structure
    rtcData.LastWiFiChannel = 0;
    memcpy( rtcData.bssid, "000000", 6 ); // Copy 6 bytes of BSSID (AP's MAC address)
    rtcData.crc32 = 0;
    rtcData.lastUTCTime = 0;

// Write rtcData back to RTC memory
    if (ESP.rtcUserMemoryWrite(0, (uint32_t*) &rtcData, sizeof(rtcData))) return true;
    else return false;
}

//  ESP8266
void GoingToSleep(byte Time_minutes = 0, unsigned long currUTime = 0 ) {
    rtcData.lastUTCTime = currUTime;
    keep_IP_address();
    RTC_write();
    ESP.deepSleep( Time_minutes * 60 * 1000000);          // time in minutes converted to microseconds
}
*/

// ESP32
void GoingToSleep(byte Time_minutes = 0, unsigned long currUTime = 0 ) {
  // Store counter to the Preferences
    preferences.putULong("UTCTime", currUTime);
    keep_IP_address();

  // Close the Preferences
    preferences.end();

  // Configure Wake Up
    if ( Ext1WakeUP>=0 && (Time_minutes ==0 || Time_minutes > 5) ) {
        const uint64_t ext_wakeup_pin_1_mask = 1ULL << Ext1WakeUP;
        esp_sleep_enable_ext1_wakeup(ext_wakeup_pin_1_mask, ESP_EXT1_WAKEUP_ALL_LOW);
//  Example using two PINs for external Wake UP 
//      const int ext_wakeup_pin_1 = 2;
//      const uint64_t ext_wakeup_pin_1_mask = 1ULL << ext_wakeup_pin_1;
//      const int ext_wakeup_pin_2 = 4;
//      const uint64_t ext_wakeup_pin_2_mask = 1ULL << ext_wakeup_pin_2;
//      printf("Enabling EXT1 wakeup on pins GPIO%d, GPIO%d\n", ext_wakeup_pin_1, ext_wakeup_pin_2);
//      esp_sleep_enable_ext1_wakeup(ext_wakeup_pin_1_mask | ext_wakeup_pin_2_mask, ESP_EXT1_WAKEUP_ANY_HIGH);

    }

    if (config.HW_Module) ulp_action(1000000);                                          // 10 second loop

    if (Time_minutes > 0) esp_sleep_enable_timer_wakeup(Time_minutes * 60  * 1000000);  // time in minutes converted to microseconds
    esp_deep_sleep_start();
}


double ReadVoltage(byte pin){
  double reading = analogRead(pin); // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
  if(reading < 1 || reading > 4095) return -1;
  //return -0.000000000009824 * pow(reading,3) + 0.000000016557283 * pow(reading,2) + 0.000854596860691 * reading + 0.065440348345433;
  return -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
} // Added an improved polynomial, use either, comment out as required


float getBattLevel() {                                      // return Battery level in Percentage [0 - 100%]
#ifdef IP5306
    return float(GetIP5306BattLevel());
#else
    float voltage = 0.0;                                    // Input Voltage [v]
    for(int i = 0; i < Number_of_measures; i++) {
        voltage += ReadVoltage(36);
        delay(1);
    };
    voltage = voltage / Number_of_measures;
    voltage = (voltage * 2) + config.LDO_Corr;              // "* 2" multiplier required when using a 50K + 50K Resistor divider between VCC and ADC. 
    if (config.DEBUG) Serial.println("Averaged and Corrected Voltage: " + String(voltage));
    if (voltage > Batt_Max ) {
        if (config.DEBUG) Serial.println("Voltage will be truncated to Batt_Max: " + String(Batt_Max));
        voltage = Batt_Max;
    }
    return ((voltage - Batt_Min) / (Batt_Max - Batt_Min)) * 100.0;
#endif
}

long getRSSI() {
    // return WiFi RSSI Strength signal [dBm]
    long r = 0;

    for(int i = 0; i < Number_of_measures; i++) {
        r += WiFi.RSSI();
    }
    r = r /Number_of_measures;
    return r;
}


void ESPRestart() {
    if (config.DEBUG) Serial.println("Restarting ESP...");
    delay(100);
    esp_restart();
}

static const String RESET_REASON_to_string[] = {
    "NO_MEAN",                 /**<=  0, OK*/
    "POWERON_RESET",           /**<=  1, Vbat power on reset*/
    "SW_RESET",                /**<=  3, Software reset digital core*/
    "OWDT_RESET",              /**<=  4, Legacy watch dog reset digital core*/
    "Deep-Sleep Wake",         /**<=  5, Deep Sleep reset digital core  original msg -> "DEEPSLEEP_RESET"*/
    "SDIO_RESET",              /**<=  6, Reset by SLC module, reset digital core*/
    "TG0WDT_SYS_RESET",        /**<=  7, Timer Group0 Watch dog reset digital core*/
    "TG1WDT_SYS_RESET",        /**<=  8, Timer Group1 Watch dog reset digital core*/
    "RTCWDT_SYS_RESET",        /**<=  9, RTC Watch dog Reset digital core*/
    "INTRUSION_RESET",         /**<= 10, Instrusion tested to reset CPU*/
    "TGWDT_CPU_RESET",         /**<= 11, Time Group reset CPU*/
    "SW_CPU_RESET",            /**<= 12, Software reset CPU*/
    "RTCWDT_CPU_RESET",        /**<= 13, RTC Watch dog Reset CPU*/
    "EXT_CPU_RESET",           /**<= 14, for APP CPU, reseted by PRO CPU*/
    "RTCWDT_BROWN_OUT_RESET",  /**<= 15, Reset when the vdd voltage is not stable*/
    "RTCWDT_RTC_RESET"         /**<= 16  RTC Watch dog reset digital core and rtc module*/
};

String ESPWakeUpReason() {    // WAKEUP_REASON
  return RESET_REASON_to_string[rtc_get_reset_reason(0)];
}

/*
void FormatConfig() {                                   // WARNING!! To be used only as last resource!!!
    Serial.println(ESP.eraseConfig());
    RTC_reset();
    delay(100);
    ESP.reset();
}
*/

void blink_LED(unsigned int slot, int bl_LED = LED_ESP, bool LED_OFF = config.LED) { // slot range 1 to 10 =>> 3000/300
    if (bl_LED>=0) {
        now_millis = millis() % Pace_millis;
        if (now_millis > LED_millis*(slot-1) && now_millis < LED_millis*slot-LED_millis/2) digitalWrite(bl_LED, !LED_OFF); // Turn LED on
        now_millis = (millis()-LED_millis/3) % Pace_millis;
        if (now_millis > LED_millis*(slot-1) && now_millis < LED_millis*slot-LED_millis/2) digitalWrite(bl_LED, LED_OFF); // Turn LED on
    }
}

void flash_LED(unsigned int n_flash = 1, int fl_LED = LED_ESP, bool LED_OFF = config.LED) {
    if (fl_LED>=0) {
        for (size_t i = 0; i < n_flash; i++) {
            digitalWrite(fl_LED, !LED_OFF);             // Turn LED on
            delay(LED_millis/3);
            digitalWrite(fl_LED, LED_OFF);              // Turn LED off
            yield();
            delay(LED_millis);
        }
    }
}

void Buzz(unsigned int n_beeps = 1, unsigned long buzz_time = BUZZER_millis ) {
    if (BUZZER>=0) {
        for (size_t i = 0; i < n_beeps; i++) {
            digitalWrite(BUZZER, HIGH);                 // Turn Buzzer on
            delay(BUZZER_millis);
            digitalWrite(BUZZER, LOW);                  // Turn Buzzer off
            yield();
            delay(BUZZER_millis);
        }
    }
}


void hw_setup() {
  // Initiate Power Management CHIP      
    #ifdef IP5306
        ip5306_setup();
    #endif

  // Output GPIOs
    if (LED_ESP>=0) {
        pinMode(LED_ESP, OUTPUT);
        digitalWrite(LED_ESP, LOW);                     // initialize LED off
    }
    if (BUZZER>=0) {
        pinMode(BUZZER, OUTPUT);
        digitalWrite(BUZZER, LOW);                      // initialize BUZZER off
    }

  // Input GPIOs
    if (Def_Config>=0) {
        pinMode(Def_Config, INPUT_PULLUP);
        if (!digitalRead(Def_Config)) {
            delay(5000);
            if (!digitalRead(Def_Config)) {
                storage_reset();
                RTC_reset();
                ESPRestart();
            }
        }
    }

    analogSetPinAttenuation(36,ADC_11db);   // ADC_11db provides an attenuation so that IN/OUT = 1 / 3.6.
                                            // An input of 3 volts is reduced to 0.833 volts before ADC measurement
    adcAttachPin(36);                       // S_VP  -- GPIO36, ADC_PRE_AMP, ADC1_CH0, RTC_GPIO0


  // Disable BT (most of project won't use it) to save battery.
  //  esp_bt_controller_disable();

  // Non.Volatile Memory
    preferences.begin("my-app", false);
    Serial.println("Stored UTCTime: " + String(preferences.getULong("UTCTime")));

}

void hw_loop() {
  // LED handling usefull if you need to identify the unit from many
    //if (LED_ESP>=0) digitalWrite(LED_ESP, boolean(config.LED));
    yield();
}
