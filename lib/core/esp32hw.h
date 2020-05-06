#include <wifi.h>
#include <Preferences.h>


uint32_t getChipId();
String HEXtoUpperString(uint32_t hexval, uint hexlen);

//System Parameters
String ChipID=HEXtoUpperString(getChipId(), 6);
#define ESP_SSID String("ESP-" + ChipID)                // SSID used as Acces Point
#define Number_of_measures 5                           // Number of value samples (measurements) to calculate average

Preferences preferences;                    // Preferences library is wrapper around Non-volatile storage on ESP32 processor.
/*
// ADC to internal voltage
#if Using_ADC == false
    ADC_MODE(ADC_VCC)                       // Get voltage from Internal ADC
#endif
*/

// Battery & ESP Voltage
#define Batt_Max float(4.1)                 // Battery Highest voltage.  [v]
#define Batt_Min float(2.8)                 // Battery lowest voltage.   [v]
#define Vcc float(3.3)                      // Theoretical/Typical ESP voltage. [v]
#define VADC_MAX float(1.0)                 // Maximum ADC Voltage input
float voltage = 0.0;                        // Input Voltage [v]
float Batt_Level = 100.0;                   // Battery level [0%-100%]

// Timers for millis used on Sleeping and LED flash
unsigned long ONTime_Offset=0;
unsigned long Extend_time=0;
unsigned long now_millis=0;
unsigned long Pace_millis=3000;
unsigned long LED_millis=300;               // 10 slots available (3000 / 300)
unsigned long BUZZER_millis=500;            // 6 Buzz beeps maximum  (3000 / 500)


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

/*
//  ESP8266
void GoingToSleep(byte Time_minutes = 0, unsigned long currUTime = 0 ) {
    rtcData.lastUTCTime = currUTime;
    RTC_write();
    ESP.deepSleep( Time_minutes * 60 * 1000000);          // time in minutes converted to microseconds
}
*/

// ESP32
void GoingToSleep(byte Time_minutes = 0, unsigned long currUTime = 0 ) {
  // Store counter to the Preferences
  preferences.putULong("UTCTime", currUTime);

  // Close the Preferences
  preferences.end();

  if (Time_minutes > 0) esp_sleep_enable_timer_wakeup(Time_minutes * 60  * 1000000);  // time in minutes converted to microseconds
  esp_deep_sleep_start();
}


double ReadVoltage(byte pin){
  double reading = analogRead(pin); // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
  if(reading < 1 || reading > 4095) return -1;
  //return -0.000000000009824 * pow(reading,3) + 0.000000016557283 * pow(reading,2) + 0.000854596860691 * reading + 0.065440348345433;
  return -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
} // Added an improved polynomial, use either, comment out as required


float getVoltage() {
    // return battery level in Percentage [0 - 100%]
    voltage = 0;
    for(int i = 0; i < Number_of_measures; i++) {
        voltage += ReadVoltage(36);
        delay(50);
    };
    voltage = voltage / Number_of_measures;
    voltage = (voltage * 2) + config.LDO_Corr;
    Serial.println("Averaged and Corrected Voltage: " + String(voltage));
    /*if (voltage > Batt_Max ) {
        Serial.println("Voltage will be truncated to Batt_Max: " + String(Batt_Max));
        voltage = Batt_Max;
    }*/
    return ((voltage - Batt_Min) / (Batt_Max - Batt_Min)) * 100.0;
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
    Serial.println("Restarting in 3 seconds...");
    delay(3000);
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
    delay(5000);
    ESP.reset();
}
*/

void blink_LED(unsigned int slot, int bl_LED = LED_esp, bool LED_OFF = config.LED) { // slot range 1 to 10 =>> 3000/300
    if (bl_LED>=0) {
        now_millis = millis() % Pace_millis;
        if (now_millis > LED_millis*(slot-1) && now_millis < LED_millis*slot-LED_millis/2 ) {
            digitalWrite(bl_LED, !LED_OFF);             // Turn LED on
            delay(LED_millis/3);
            digitalWrite(bl_LED, LED_OFF);              // Turn LED off
        }
    }
}

void flash_LED(unsigned int n_flash = 1, int fl_LED = LED_esp, bool LED_OFF = config.LED) {
    if (fl_LED>=0) {
        for (size_t i = 0; i < n_flash; i++) {
            digitalWrite(fl_LED, !LED_OFF);             // Turn LED on
            delay(LED_millis/3);
            digitalWrite(fl_LED, LED_OFF);              // Turn LED off
            delay(LED_millis/3);
        }
    }
}

void Buzz(unsigned int n_beeps = 1) {                   // number of beeps 1 to 6 =>> 3000/500
    if (BUZZER>=0) {
        for (size_t i = 0; i < n_beeps; i++) {
            digitalWrite(BUZZER, HIGH);                 // Turn Buzzer on
            delay(BUZZER_millis/6);
            digitalWrite(BUZZER, LOW);                  // Turn Buzzer off
            delay(BUZZER_millis/6);
        }
    }
}


void hw_setup() {
  // Output GPIOs
      if (LED_esp>=0) {
          pinMode(LED_esp, OUTPUT);
          digitalWrite(LED_esp, boolean(config.LED));  // initialize LED off
      }
      if (BUZZER>=0) {
          pinMode(BUZZER, OUTPUT);
          digitalWrite(BUZZER, LOW);                    // initialize BUZZER off
      }

  // Input GPIOs
      analogSetPinAttenuation(36,ADC_11db); // ADC_11db provides an attenuation so that IN/OUT = 1 / 3.6.
                                            // An input of 3 volts is reduced to 0.833 volts before ADC measurement
      adcAttachPin(36);                     // S_VP  -- GPIO36, ADC_PRE_AMP, ADC1_CH0, RTC_GPIO0


  // Non.Volatile Memory
      preferences.begin("my-app", false);
      Serial.println("Stored UTCTime: " + String(preferences.getULong("UTCTime")));

}

void hw_loop() {
  // LED handling usefull if you need to identify the unit from many
      if (LED_esp>=0) digitalWrite(LED_esp, boolean(config.LED));
}
