#include <EEPROM.h>
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
#include <ArduinoJson.h>
#define EEPROMZize 2048

//
// Create JSON object and related variables
//
    String JsonString="";
    StaticJsonBuffer<1024> MainjsonBuffer;
    JsonObject& root = MainjsonBuffer.createObject();


//
//  Create data Struct that will be store on RTC memmory
//
struct {
  unsigned long lastUTCTime = 0;
  unsigned int LastWiFiChannel = 1;
} rtcData;


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

void WriteStringToEEPROM(int beginaddress, String string) {
  char  charBuf[string.length() + 1];
  string.toCharArray(charBuf, string.length() + 1);
  for (unsigned int t =  0; t < sizeof(charBuf); t++)
  {
    EEPROM.write(beginaddress + t, charBuf[t]);
  }
}

String  ReadStringFromEEPROM(int beginaddress, int SizeofString = 1024) {
  volatile int counter = 0;
  char rChar;
  String retString = "";
  while (1)
  {
    rChar = EEPROM.read(beginaddress + counter);
    if (rChar == 0) break;
    if (counter > SizeofString) break;
    counter++;
    retString.concat(rChar);
  }
  Serial.println("READ Size of String: " + String(counter));
  return retString;
}

// Adding function with project's customized STORAGE actions
#include <custostore.h>

//
// MAIN Functions
//

void JSON_TO_STRUCT() {
    config.DeviceName                = (String) root.get<const char*>("DeviceName")      ;
    config.Location                  = (String) root.get<const char*>("Location")        ;
    config.ClientID                  = (String) root.get<const char*>("ClientID")        ;
    config.ONTime                    = root["ONTime"]                                    ;
    config.SLEEPTime                 = root["SLEEPTime"]                                 ;
    config.DEEPSLEEP                 = root["DEEPSLEEP"]                                 ;
    config.LED                       = root["LED"]                                       ;
    config.TELNET                    = root["TELNET"]                                    ;
    config.OTA                       = root["OTA"]                                       ;
    config.WEB                       = root["WEB"]                                       ;
    config.Remote_Allow              = root["Remote_Allow"]                              ;
    config.STAMode                   = root["STAMode"]                                   ;
    config.ssid                      = (String) root.get<const char*>("ssid")            ;
    config.WiFiKey                   = (String) root.get<const char*>("WiFiKey")         ;
    config.dhcp                      = root["dhcp"]                                      ;
    config.IP[0]                     = root["IP0"]                                       ;
    config.IP[1]                     = root["IP1"]                                       ;
    config.IP[2]                     = root["IP2"]                                       ;
    config.IP[3]                     = root["IP3"]                                       ;
    config.Netmask[0]                = root["Mask0"]                                     ;
    config.Netmask[1]                = root["Mask1"]                                     ;
    config.Netmask[2]                = root["Mask2"]                                     ;
    config.Netmask[3]                = root["Mask3"]                                     ;
    config.Gateway[0]                = root["GW0"]                                       ;
    config.Gateway[1]                = root["GW1"]                                       ;
    config.Gateway[2]                = root["GW2"]                                       ;
    config.Gateway[3]                = root["GW3"]                                       ;
    config.NTPServerName             = (String) root.get<const char*>("NTPServerName")   ;
    config.TimeZone                  = root["TimeZone"]                                  ;
    config.Update_Time_Via_NTP_Every = root["Update_Time_Via_NTP_Every"]                 ;
    config.isDayLightSaving          = root["isDayLightSaving"]                          ;
    config.MQTT_Server               = (String) root.get<const char*>("MQTT_Server")     ;
    config.MQTT_Port                 = root["MQTT_Port"]                                 ;
    config.MQTT_User                 = (String) root.get<const char*>("MQTT_User")       ;
    config.MQTT_Password             = (String) root.get<const char*>("MQTT_Password")   ;
    config.UPDATE_Server             = (String) root.get<const char*>("UPDATE_Server")   ;
    config.UPDATE_Port               = root["UPDATE_Port"]                               ;
    config.UPDATE_User               = (String) root.get<const char*>("UPDATE_User")     ;
    config.UPDATE_Password           = (String) root.get<const char*>("UPDATE_Password") ;
    config.Temp_Corr                 = root["Temp_Corr"]                                 ;
    custo_JSON_TO_STRUCT();
}

void STRUCT_TO_JSON() {
    root["DeviceName"]                = config.DeviceName                 ;
    root["Location"]                  = config.Location                   ;
    root["ClientID"]                  = config.ClientID                   ;
    root["ONTime"]                    = config.ONTime                     ;
    root["SLEEPTime"]                 = config.SLEEPTime                  ;
    root["DEEPSLEEP"]                 = config.DEEPSLEEP                  ;
    root["LED"]                       = config.LED                        ;
    root["TELNET"]                    = config.TELNET                     ;
    root["OTA"]                       = config.OTA                        ;
    root["WEB"]                       = config.WEB                        ;
    root["Remote_Allow"]              = config.Remote_Allow               ;
    root["STAMode"]                   = config.STAMode                    ;
    root["ssid"]                      = config.ssid                       ;
    root["WiFiKey"]                   = config.WiFiKey                    ;
    root["dhcp"]                      = config.dhcp                       ;
    root["IP0"]                       = config.IP[0]                      ;
    root["IP1"]                       = config.IP[1]                      ;
    root["IP2"]                       = config.IP[2]                      ;
    root["IP3"]                       = config.IP[3]                      ;
    root["Mask0"]                     = config.Netmask[0]                 ;
    root["Mask1"]                     = config.Netmask[1]                 ;
    root["Mask2"]                     = config.Netmask[2]                 ;
    root["Mask3"]                     = config.Netmask[3]                 ;
    root["GW0"]                       = config.Gateway[0]                 ;
    root["GW1"]                       = config.Gateway[1]                 ;
    root["GW2"]                       = config.Gateway[2]                 ;
    root["GW3"]                       = config.Gateway[3]                 ;
    root["NTPServerName"]             = config.NTPServerName              ;
    root["TimeZone"]                  = config.TimeZone                   ;
    root["Update_Time_Via_NTP_Every"] = config.Update_Time_Via_NTP_Every  ;
    root["isDayLightSaving"]          = config.isDayLightSaving           ;
    root["MQTT_Server"]               = config.MQTT_Server                ;
    root["MQTT_Port"]                 = config.MQTT_Port                  ;
    root["MQTT_User"]                 = config.MQTT_User                  ;
    root["MQTT_Password"]             = config.MQTT_Password              ;
    root["UPDATE_Server"]             = config.UPDATE_Server              ;
    root["UPDATE_Port"]               = config.UPDATE_Port                ;
    root["UPDATE_User"]               = config.UPDATE_User                ;
    root["UPDATE_Password"]           = config.UPDATE_Password            ;
    root["Temp_Corr"]                 = config.Temp_Corr                  ;
    custo_STRUCT_TO_JSON();
}

void storage_print() {

  Serial.println("Printing Config");
  Serial.printf("Device Name: %s and Location: %s\n", config.DeviceName.c_str(), config.Location.c_str());
  Serial.printf("ON time[sec]: %d  SLEEP Time[min]: %d  DEEPSLEEP enabled: %d\n", config.ONTime, config.SLEEPTime, config.DEEPSLEEP);
  Serial.printf("LED enabled: %d - TELNET enabled: %d - OTA enabled: %d - WEB enabled: %d\n", config.LED, config.TELNET, config.OTA, config.WEB);
  Serial.printf("WiFi STA Mode: %d\n", config.STAMode);
  Serial.printf("WiFi SSID: %s\n", config.ssid.c_str());
  Serial.printf("WiFi Key: %s\n", config.WiFiKey.c_str());

  Serial.printf("DHCP enabled: %d\n", config.dhcp);
  if(!config.dhcp) {
      Serial.printf("IP: %d.%d.%d.%d\n", config.IP[0],config.IP[1],config.IP[2],config.IP[3]);
      Serial.printf("Mask: %d.%d.%d.%d\n", config.Netmask[0],config.Netmask[1],config.Netmask[2],config.Netmask[3]);
      Serial.printf("Gateway: %d.%d.%d.%d\n", config.Gateway[0],config.Gateway[1],config.Gateway[2],config.Gateway[3]);
  }
  Serial.printf("NTP Server Name: %s\n", config.NTPServerName.c_str());
  Serial.printf("NTP update every %ld minutes.\n", config.Update_Time_Via_NTP_Every);
  Serial.printf("Timezone: %ld  -  DayLight: %d\n", config.TimeZone, config.isDayLightSaving);
}


boolean storage_read() {
  Serial.println("Reading Configuration");
  if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' && EEPROMReadlong(3) > 2)
  {
    Serial.println("Configurarion Found!");
    String tmpstrg = ReadStringFromEEPROM(16);
    //Serial.println(tmpstrg);
    //converting string to array of chars + null char
    char json[tmpstrg.length()+1];
    strcpy(json, tmpstrg.c_str());
    json[tmpstrg.length()+1] = '\0';
    /*Serial.println("JSON string value: ");
    Serial.println(json);*/
    MainjsonBuffer.clear();
    JsonObject& root = MainjsonBuffer.parseObject((char*)json);
    if (root.success()) {
      JSON_TO_STRUCT();
      /*JsonString="";
      Serial.println("Parsed JSON: ");
      root.printTo(JsonString);
      Serial.println(JsonString);*/
      return true;
    }
    else {
      Serial.println("Error while parsing !!!!");
      return false;
    }
  }
  else
  {
    Serial.println("Configurarion NOT FOUND!!!!");
    return false;
  }
}


void storage_write() {
  Serial.println("Writing Config");
  MainjsonBuffer.clear();
  JsonObject& root = MainjsonBuffer.createObject();
  STRUCT_TO_JSON();
  JsonString="";                        // do not comment this line!
  root.printTo(JsonString);             // do not comment this line!
  /*Serial.println("JSON string: ");    // just for debug purpose
  Serial.println(JsonString);*/         // just for debug purpose

  EEPROM.write(0, 'C');
  EEPROM.write(1, 'F');
  EEPROM.write(2, 'G');
  EEPROMWritelong(3, JsonString.length());
  //Serial.println("Size of STORAGE String: " + String(JsonString.length()));
  WriteStringToEEPROM(16, JsonString);
  EEPROM.commit();
}


void storage_reset() {

  Serial.println("Reseting Config");
  EEPROM.write(0, 'R');
  EEPROM.write(1, 'S');
  EEPROM.write(2, 'T');
  for (size_t i = 3; i < (EEPROMZize-1); i++) {
    EEPROM.write(i, 0);
  }

  EEPROM.commit();
}

void storage_setup() {
    bool CFG_saved = false;
    EEPROM.begin(EEPROMZize);     // define an EEPROM space of 2048 Bytes to store data
    //storage_reset();            // Hack to reset storage during boot
    config_defaults();
    CFG_saved = storage_read();   //When a configuration exists, it uses stored values
    if (!CFG_saved) {             // If NOT, it Set DEFAULT VALUES to "config" struct
        config_defaults();
        storage_write();
    }
    storage_print();
}
