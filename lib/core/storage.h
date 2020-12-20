/*
## Store any struct in flash ##
 Kudos for this author: Robert Duality4Y [GitHub]
 https://github.com/esp8266/Arduino/issues/1539
*/
#include <EEPROM.h>
#define EEPROMZize 4096                     // 4KB
#define Mem_Start_Pos 1024                  // Memory starting point Position for the space to write/read data

#include <ArduinoJson.h>

bool Load_Default = true;                   // Will it Load Default values? It means it's running for first time.  


//
//  AUXILIAR functions to handle EEPROM
//
void EEPROMWritelong(int address, long value) {
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

long EEPROMReadlong(long address) {
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void loadStruct(void *data_dest, size_t size, int start_address) {
    for(size_t i = 0; i < size; i++) {
        char data = EEPROM.read(i + start_address);
        ((char *)data_dest)[i] = data;
    }
}

void storeStruct(void *data_source, size_t size, int start_address) {
    for(size_t i = 0; i < size; i++) {
        char data = ((char *)data_source)[i];
        EEPROM.write(i + start_address, data);
    }
    EEPROM.commit();
}


//
//  CONFIG Struct
//
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
  bool APMode;
  char SSID[32];
  char WiFiKey[32];
  bool DHCP;
  byte IP[4];
  byte Netmask[4];
  byte Gateway[4];
  byte DNS_IP[4];
  char NTPServerName[128];
  long TimeZone;
  unsigned long Update_Time_Via_NTP_Every;
  bool isDayLightSaving;
  char MQTT_Server[128];
  long MQTT_Port;
  bool MQTT_Secure;
  char MQTT_User[32];
  char MQTT_Password[32];
  char UPDATE_Server[128];
  long UPDATE_Port;
  char UPDATE_User[16];
  char UPDATE_Password[32];
  char SIMCardPIN[5];
  char APN[128];
  char MODEM_User[32];
  char MODEM_Password[32];
  char WEB_User[16];
  char WEB_Password[32];
  float Temp_Corr;
  float LDO_Corr;
  bool HW_Module;
  bool HASSIO_CFG;
  bool DEBUG;
  bool SW_Upgraded;
  bool SWITCH_Default;
  unsigned int UPPER_LEVEL;
  unsigned int LOWER_LEVEL;
  long MIN_TRAVEL;
  long MAX_TRAVEL;
  double Voltage_Multiplier;
  double Current_Multiplier;
  double Power_Multiplier;
} config;


#include <def_conf.h>


//
//  STORAGE functions
//
void storage_print() {
    Serial.printf("Config Size: [%d bytes]\n", sizeof(config));
    if (sizeof(config) + 16 > Mem_Start_Pos) Serial.println ("WARNING: Memory zones overlapinng!!");
    Serial.printf("Device Name: %s and Location: %s\n", config.DeviceName, config.Location);
    Serial.printf("ON time[sec]: %d  -  SLEEP Time[min]: %d -  DEEPSLEEP enabled: %d\n", config.ONTime, config.SLEEPTime, config.DEEPSLEEP);
    Serial.printf("LED enabled: %d   -  TELNET enabled: %d  -  OTA enabled: %d  -  WEB enabled: %d\n", config.LED, config.TELNET, config.OTA, config.WEB);
    Serial.printf("WiFi AP Mode: %d  -  WiFi STA Mode: %d   -  WiFi SSID: %s  -  WiFi Key: %s\n", config.APMode, config.STAMode, config.SSID, config.WiFiKey);
  
    Serial.printf("DHCP enabled: %d\n", config.DHCP);
    if(!config.DHCP) {
      Serial.printf("IP: %d.%d.%d.%d\t", config.IP[0],config.IP[1],config.IP[2],config.IP[3]);
      Serial.printf("Mask: %d.%d.%d.%d\t", config.Netmask[0],config.Netmask[1],config.Netmask[2],config.Netmask[3]);
      Serial.printf("Gateway: %d.%d.%d.%d\n", config.Gateway[0],config.Gateway[1],config.Gateway[2],config.Gateway[3]);
      Serial.printf("DNS IP: %d.%d.%d.%d\t", config.DNS_IP[0],config.DNS_IP[1],config.DNS_IP[2],config.DNS_IP[3]);
    }
    Serial.printf("MQTT Server: %s  -  Port: %ld  -  Secure: %d  -  ", config.MQTT_Server, config.MQTT_Port, config.MQTT_Secure);
    Serial.printf("MQTT User: %s  -  MQTT Pass: %s\n", config.MQTT_User, config.MQTT_Password);
    Serial.printf("NTP Server Name: %s\t", config.NTPServerName);
    Serial.printf("NTP update every %ld minutes.\t", config.Update_Time_Via_NTP_Every);
    Serial.printf("Timezone: %ld  -  DayLight: %d\n", config.TimeZone, config.isDayLightSaving);

    Serial.printf("HW Module: %d  -  Remote Allowed: %d  -  WEB User: %s  -  WEB Pass: %s\n", config.HW_Module, config.Remote_Allow, config.WEB_User, config.WEB_Password);
    Serial.printf("SWITCH default: %d  -  Temperature Correction: %.2f  -  Voltage Correction: %.2f\n", config.SWITCH_Default, config.Temp_Corr, config.LDO_Corr);
}

boolean storage_read() {
    //if (config.DEBUG) Serial.println("Reading Configuration");
    if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' && EEPROMReadlong(3) > 2 && EEPROMReadlong(3) == sizeof(config)) {
        //if (config.DEBUG) Serial.println("Configurarion Found!");
        loadStruct(&config, EEPROMReadlong(3), 15);     // I just decided that it will read/write after address 15
      return false;   // "false" means no error
    }
    else {
    if (config.DEBUG) Serial.println("Configurarion NOT FOUND!!!!");
        //Serial.println("Value of 0,1,2: " + String(EEPROM.read(0)) + String(EEPROM.read(1)) + String(EEPROM.read(2)));
        //Serial.println("Value of 3: " + String(EEPROMReadlong(3)));
    return true;      // "true" means failed to read configuration
  }
}

void storage_write() {
  if (config.DEBUG) Serial.println("Writing Config");
  EEPROM.write(0, 'C');
  EEPROM.write(1, 'F');
  EEPROM.write(2, 'G');
  EEPROMWritelong(3, sizeof(config));
  //Serial.println("Value of 3 WRITE: " + String(sizeof(config)));

  storeStruct(&config, sizeof(config), 15);             // I just decided that it will read/write after address 15
  //Serial.println("Value of 0,1,2 READ: " + String(EEPROM.read(0)) + String(EEPROM.read(1)) + String(EEPROM.read(2)));
  //Serial.println("Value of 3 READ: " + String(EEPROMReadlong(3)));
}

void storage_reset() {
  if (config.DEBUG) Serial.println("Reseting Config");
  EEPROM.write(0, 'R');
  EEPROM.write(1, 'S');
  EEPROM.write(2, 'T');
  for (size_t i = 3; i < (EEPROMZize-1); i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

void storage_setup() {
    EEPROM.begin(EEPROMZize);         // define an EEPROM space of 2048 Bytes to store data
    //storage_reset();                // Hack to reset storage during boot
    config_defaults();
    Load_Default = storage_read();    //When a configuration exists, it uses stored values
    if (Load_Default) {               // If NOT, it load DEFAULT VALUES to "config" struct
        config_defaults();
        storage_write();
    }
    if (config.DEBUG) storage_print();
}
