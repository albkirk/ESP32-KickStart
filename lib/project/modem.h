// **** Modem code definition here ...

// ModemSate Names
static const String Modem_state_Name[] = {
    "Power OFF",                    // 0
    "Power ON",                     // 1
    "Network Connected",            // 2
    "IP Connected",                 // 3
};

// SimStatus Names
static const String SimStatus_Name[] = {
    "SIM_ERROR",                    // 0
    "SIM_READY",                    // 1
    "SIM_LOCKED",                   // 2
    "SIM_ANTITHEFT_LOCKED",         // 3
};

// RegStatus Names
static const String RegStatus_Name[] = {
    "REG_NO_RESULT",                // -1
    "REG_UNREGISTERED",             // 0
    "REG_OK_HOME",                  // 1
    "REG_SEARCHING",                // 2
    "REG_DENIED",                   // 3
    "REG_UNKNOWN",                  // 4
    "REG_OK_ROAMING"                // 5
};

#define Modem_WEB                                   // Flag to load Modem Page on WEB Service
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#define TINY_GSM_RX_BUFFER 1024                     // Set RX buffer to 1Kb

#ifdef ESP8266
    #include "SoftwareSerial.h"                     // for use with ESP8266
    SoftwareSerial SerialAT();                      // for use with ESP8266

#elif ESP32
    HardwareSerial serialGsm(1);                    // for use with ESP32 (1/2)
    #define SerialAT serialGsm                      // for use with ESP32 (2/2)

#else
    #error “Unsupported board selected!”
#endif

#include <TinyGsmClient.h>
TinyGsm modem(SerialAT);                            // Modem to Serial commnunication instance
TinyGsmClient modemWebClient(modem);                   // Modem WEB client instance (for HTTP or MQTT usage)

int Modem_state = 0;                                 // Modem State
String ModemName;                                   // Will get it using modem.getModemName();
String ModemInfo;                                   // will get it using modem.getModemInfo();
String ModemIMEI;                                   // will get it using modem.getIMEI();
String SIMCardIMSI;                                 // SIM Card IMSI
String SIMCardCCID;                                 // SIM Card CCID



// **** Modem code functions here ...
String  RegStatus_string(RegStatus regstatus){
   return RegStatus_Name[map(regstatus, -1, 5, 0 , 6)];
}

int CSQ_to_RSSI (int CSQ_value) {
/*
CSQ Value   Rx signal strength level (RSSI)
0           -110 dBm or less
1           -109 dBm <= rssi < -107 dBm
2           -107 dBm <= rssi < -105 dBm
3…30        -105dBm <= rssi < -48 dBm
31          -48dBm <= rssi
99          Not known or not detectable
*/
if (CSQ_value == 99) return 99;
if (CSQ_value == 0) return -110;
if (CSQ_value == 1) return -108;
if (CSQ_value == 2) return -106;
return int(map(CSQ_value, 3, 31, -105, -48));
}

bool MODEM_Set_PIN() {
    // Unlock your SIM card with a PIN if needed
    if (strlen(config.SIMCardPIN) == 4 && modem.getSimStatus() != 3 ) {
        return modem.simUnlock(config.SIMCardPIN);
    }
    return false;
}

void MODEM_Info() {
    // ModemName   = modem.getModemName();
    ModemInfo   = modem.getModemInfo();
    ModemIMEI   = modem.getIMEI();
    SIMCardIMSI = modem.getIMSI();
    SIMCardCCID = modem.getSimCCID();
}

bool MODEM_ON(uint32_t time_delay = 1000) {
    // Set-up modem reset, enable and power pins
    bool res;
    //MODEM_RST & IP5306 IRQ: HIGH"
    if (MODEM_RST>=0)     digitalWrite(MODEM_RST, HIGH);

    // Enabling SY8089 4V4 for SIM800 (crashing when in battery)
    if (MODEM_POWER_ON>=0) { digitalWrite(MODEM_POWER_ON, HIGH); delay(time_delay/10); } 
  
    // POWER Key cycle
    if (MODEM_PWKEY>=0) {
        digitalWrite(MODEM_PWKEY, LOW);             // turning modem OFF
        delay(time_delay);
        digitalWrite(MODEM_PWKEY, HIGH);            // turning modem ON
    }
    Modem_state = 1;
    //if (config.DEBUG) Serial.println("MODEM_ON");
    res = modem.init();
    if (res) {
        telnet_println("Modem INIT !");
        if (modem.waitForNetwork(10000U)) {
            modem.testAT();                             // send a simple AT command to clean the buffer
            Modem_state = 2;
            //if (config.DEBUG) Serial.println("MODEM_Ready");
        }
        else telnet_println("Modem ON Network error");
    }
    else telnet_println("Modem ON Init error");
    telnet_println("Modem State: " + Modem_state_Name[Modem_state] + "\t REG State: " + RegStatus_string(modem.getRegistrationStatus()));
    return res;

}

void MODEM_OFF() {
    if(modem.radioOff()) {
        //if (config.DEBUG) Serial.println("Radio off");
        if (MODEM_PWKEY>=0)    digitalWrite(MODEM_PWKEY, HIGH);   // turn of modem in case its ON from previous state
        if (MODEM_POWER_ON>=0) digitalWrite(MODEM_POWER_ON, LOW); // turn of modem psu in case its ON from previous state
        if (MODEM_RST>=0)      digitalWrite(MODEM_RST, HIGH);     // Keep IRQ high ? (or not to save power?)
        telnet_println("Modem Power OFF");
        Modem_state = 0;
    }
}

bool MODEM_Connect() {
    bool res = false;
    RegStatus regtemp = modem.getRegistrationStatus();
    if ( regtemp == 1 || regtemp == 5 ) {
        Modem_state = 2;
        res = modem.gprsConnect(config.APN, config.MODEM_User, config.MODEM_Password);
        if(res) {
            Modem_state = 3;
            Celular_Connected = true;
            //Serial.println("MODEM_Connect");
        }
        else if (config.DEBUG) Serial.println("MODEM_Connect() error");
    }
    else if (regtemp != -1 ) Modem_state = 1;
    else Modem_state = 0;
    telnet_println("Modem State: " + Modem_state_Name[Modem_state] + "\t REG State: " + RegStatus_string(modem.getRegistrationStatus()));
    return res;
}

bool MODEM_Disconnect() {
    bool res; 
    res = modem.gprsDisconnect();
    if(res) {
        Celular_Connected = false;
        //Serial.println("MODEM_Disconnect");
        RegStatus regtemp = modem.getRegistrationStatus();
        if ( regtemp == 1 || regtemp == 5 ) Modem_state = 2;
        else if (regtemp != -1 ) Modem_state = 1;
        else Modem_state = 0;
    }
    else if (config.DEBUG) Serial.println("MODEM_Disconnect() error");
    return res;
}


void MODEM_mqtt_client() {
        if (config.MQTT_Secure) MQTTclient.setClient(modemWebClient);
        else MQTTclient.setClient(modemWebClient);
}


void modem_setup() {
 // Output GPIOs
    if (MODEM_PWKEY>=0)     pinMode(MODEM_PWKEY, OUTPUT);
    if (MODEM_POWER_ON>=0)  pinMode(MODEM_POWER_ON, OUTPUT);
    if (MODEM_RST>=0)       pinMode(MODEM_RST, OUTPUT);

    if (MODEM_PWKEY>=0)    digitalWrite(MODEM_PWKEY, HIGH);   // turn of modem in case its ON from previous state
    if (MODEM_POWER_ON>=0) digitalWrite(MODEM_POWER_ON, LOW); // turn of modem psu in case its ON from previous state
    if (MODEM_RST>=0)      digitalWrite(MODEM_RST, HIGH);     // Keep IRQ high ? (or not to save power?)
    Modem_state = 0;

 // Input GPIOs


    #ifdef ESP8266
        SerialAT.begin(9600U, SWSERIAL_8N1 , MODEM_RX, MODEM_TX);     // Use SWSERIAL_8N1 with SWserial. Use SERIAL_8N1 with  HWserial.
    #else
        SerialAT.begin(9600U, SERIAL_8N1 , MODEM_RX, MODEM_TX);       // Use SWSERIAL_8N1 with SWserial. Use SERIAL_8N1 with  HWserial.
    #endif
}


void modem_start() {
    if (MODEM_ON()) { 
        MODEM_Connect();
    }
    else if (config.DEBUG) Serial.println("Modem connection ERROR");
    telnet_println("Modem State: " + Modem_state_Name[Modem_state] + "\t REG State: " + RegStatus_string(modem.getRegistrationStatus()));
}

void modem_stop() {
    if (Modem_state >=3) MODEM_Disconnect();
    if (Modem_state >=1) MODEM_OFF();
}

void modem_loop() {
      //if (TIMER >0) if ((millis() - 3500) % (TIMER * 60000) < 5) ambient_data();      // TIMER bigger than zero on div or dog bites!!

    modem.maintain();
}
