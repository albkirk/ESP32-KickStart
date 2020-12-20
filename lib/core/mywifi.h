// Expose Espressif SDK functionality
/*
extern "C" {
#include "user_interface.h"
  typedef void (*freedom_outside_cb_t)(uint8 status);
  int  wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);
  void wifi_unregister_send_pkt_freedom_cb(void);
  int  wifi_send_pkt_freedom(uint8 *buf, int len, bool sys_seq);
}
*/

// WiFi VARIABLEs
static const String WIFI_state_Name[] = {
	"WL_IDLE_STATUS",			// 0
    "WL_NO_SSID_AVAIL",			// 1
    "WL_SCAN_COMPLETED",		// 2
    "WL_CONNECTED",				// 3
    "WL_CONNECT_FAILED",		// 4
    "WL_CONNECTION_LOST",		// 5
    "WL_DISCONNECTED"			// 6
};
int WIFI_state = WL_DISCONNECTED;
int Last_WIFI_state = WL_NO_SHIELD;
unsigned int WIFI_Retry = 180;                  // Timer to retry the WiFi connection
unsigned long WIFI_LastTime = 0;                // Last WiFi connection attempt time stamp
int WIFI_errors = 0;                            // WiFi errors Counter


// Wi-Fi functions

String WIFI_state_string(int wifistate = WIFI_state) {
    return WIFI_state_Name[WIFI_state];
}

// void wifi_disconnect() {}                                // defined on 8266_HW.h and esp32.h file

void wifi_connect() {
    //  Connect to WiFi acess point or start as Acess point
    if ( WiFi.status() != WL_CONNECTED ) {
        if (config.APMode) {
            if (config.STAMode) WiFi.mode(WIFI_AP_STA);     // Setup ESP in AP+Station  mode
            else WiFi.mode(WIFI_AP);                        // Setup ESP in AP only mode
        }
        else {
            #ifndef Modem
                config.STAMode = true;                      // To force having any conectivity to the world
            #endif
            if (config.STAMode) WiFi.mode(WIFI_STA);        // Setup ESP in Station only mode
        }

        if (config.STAMode) {
            // Handle DHCP, IP address and hostname for the shield
            if (  !config.DHCP || ( RTC_read() && (ESPWakeUpReason() == "Deep-Sleep Wake") )  ) {
            //if (!config.DHCP) {
                WiFi.persistent(true);                   // required for fast WiFi registration
                // Static IP (No DHCP) may be handy for fast WiFi registration
                IPAddress StaticIP(config.IP[0], config.IP[1], config.IP[2], config.IP[3]);
                IPAddress Gateway(config.Gateway[0], config.Gateway[1], config.Gateway[2], config.Gateway[3]);
                IPAddress Subnet(config.Netmask[0], config.Netmask[1], config.Netmask[2], config.Netmask[3]);
                IPAddress DNS(config.DNS_IP[0], config.DNS_IP[1], config.DNS_IP[2], config.DNS_IP[3]);
                WiFi.config(StaticIP, Gateway, Subnet, DNS);
            };
            wifi_hostname();
            if( RTC_read() && (ESPWakeUpReason() == "Deep-Sleep Wake") ) {
                // The RTC data was good, make a quick connection
                if (config.DEBUG) Serial.print("Waking from DeepSleep and connecting to WiFi using RTD data and Static IP... ");
                WiFi.begin( config.SSID, config.WiFiKey, rtcData.LastWiFiChannel, rtcData.bssid, true );
                WIFI_state = wifi_waitForConnectResult(2000);
                if ( WIFI_state != WL_CONNECTED ) {
                    if (config.DEBUG) Serial.println(" ---ERROR!?!. Trying using config values. ");
                    if (config.DHCP) WiFi.config((uint32_t)0x0, (uint32_t)0x0, (uint32_t)0x0, (uint32_t)0x0);
                    WiFi.begin(config.SSID, config.WiFiKey);
                    WIFI_state = wifi_waitForConnectResult(10000);
                };
            }
            else {
                // The RTC data was not valid, so make a regular connection
                if (config.DEBUG) Serial.print("NO RTD data or NOT waking from DeepSleep. Using configured WiFi values ... ");
                WiFi.begin(config.SSID, config.WiFiKey);
                WIFI_state = wifi_waitForConnectResult(10000);
            }
            if ( WIFI_state == WL_CONNECTED ) {
                if (config.DEBUG) { Serial.print("Connected to WiFi network! " + String(config.SSID) + " IP: "); Serial.println(WiFi.localIP());}

                //if (!MDNS.begin(host_name)) {             // Start the mDNS responder for "host_name.local" domain
                //    Serial.println("Error setting up MDNS responder!");
                //}
                //else Serial.println("mDNS responder started");
            }
            else if (config.DEBUG) Serial.println( "WiFI ERROR! ==> " + WIFI_state_string(WIFI_state));
        }
        if (config.APMode) {
            WiFi.softAP(ESP_SSID.c_str());
            //WiFi.softAP(config.SSID);
            if (config.DEBUG) { Serial.print("WiFi in AP mode, with IP: "); Serial.println(WiFi.softAPIP());}
        }
    }
    else WIFI_state = WL_CONNECTED;
}


void wifi_setup() {
    wifi_connect();
}

void wifi_loop() {
    if ( WiFi.status() != WL_CONNECTED ) {
        if ( millis() - WIFI_LastTime > (WIFI_Retry * 1000)) {
            WIFI_errors ++;
            if (config.DEBUG) { Serial.println( "in loop function WiFI ERROR! #: " + String(WIFI_errors) + "  ==> " + WIFI_state_string(WiFi.status())); }
            WIFI_LastTime = millis();
            wifi_connect();
        }
    }
    else {
        if (WIFI_state != WL_CONNECTED) WIFI_state = WL_CONNECTED;
    }
    yield();
}
