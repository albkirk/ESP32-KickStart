//
//Constants
//
boolean firstStart = true;                // On firststart = true, NTP will try to get a valid time
int AdminTimeOutCounter = 0;              // Counter for Disabling the AdminMode

//
// Auxiliary functions
//
String GetMacAddress(){
  uint8_t mac[6];
  char macStr[18] = {0};
  WiFi.macAddress(mac);
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],  mac[1], mac[2], mac[3], mac[4], mac[5]);
  return  String(macStr);
}

String GetAPMacAddress(){
  uint8_t mac[6];
  char macStr[18] = {0};
  WiFi.softAPmacAddress(mac);
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],  mac[1], mac[2], mac[3], mac[4], mac[5]);
  return  String(macStr);
}

// Check the Values is between 0-255
boolean checkRange(String Value){
  if (Value.toInt() < 0 || Value.toInt() > 255)
  {
    return false;
  }
  else
  {
    return true;
  }
}

// convert a single hex digit character to its integer value (from https://code.google.com/p/avr-netino/)
unsigned char h2int(char c){
  if (c >= '0' && c <= '9') {
    return ((unsigned char)c - '0');
  }
  if (c >= 'a' && c <= 'f') {
    return ((unsigned char)c - 'a' + 10);
  }
  if (c >= 'A' && c <= 'F') {
    return ((unsigned char)c - 'A' + 10);
  }
  return (0);
}

String urldecode(String input) // (based on https://code.google.com/p/avr-netino/)
{
  char c;
  String ret = "";

  for (byte t = 0; t < input.length(); t++)
  {
    c = input[t];
    if (c == '+') c = ' ';
    if (c == '%') {


      t++;
      c = input[t];
      t++;
      c = (h2int(c) << 4) | h2int(input[t]);
    }

    ret.concat(c);
  }
  return ret;

}



// MAIN Functions

  // Include the HTML, STYLE and Script "Pages"
#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTP.h"
#include "Page_MQTT.h"
#include "Page_Information.h"
#include "Page_General.h"
#include "PAGE_Wireless.h"
#include "Page_FactoryReset.h"
#include "Page_Save_Quit.h"
#ifdef Modem_WEB
  #include "Page_Modem.h"
#endif
#ifdef GPS_WEB
  #include "Page_GPS.h"
#endif

void web_setup() {

  if (config.WEB) {
        // Start HTTP Server for configuration
    MyWebServer.on ( "/", []() { MyWebServer.send ( 200, "text/html", PAGE_AdminMainPage);   }  );

    MyWebServer.on ( "/admin.html", []() { MyWebServer.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
    MyWebServer.on ( "/wireless.html", send_wireless_configuration_html );
    MyWebServer.on ( "/mqtt.html", send_mqtt_html );
    MyWebServer.on ( "/info.html", []() { MyWebServer.send ( 200, "text/html", PAGE_Information );   }  );
    MyWebServer.on ( "/ntp.html", send_NTP_configuration_html  );
    MyWebServer.on ( "/general.html", send_general_html  );
    MyWebServer.on ( "/reset.html", send_factory_reset_html  );
    MyWebServer.on ( "/savequit.html", send_save_quit_html  );

    MyWebServer.on ( "/admin/wirelessvalues", send_wireless_configuration_values_html );
    MyWebServer.on ( "/admin/connectionstate", send_connection_state_values_html );
    MyWebServer.on ( "/admin/mqttvalues", send_mqtt_values_html );
    MyWebServer.on ( "/admin/infovalues", send_information_values_html );
    MyWebServer.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
    MyWebServer.on ( "/admin/generalvalues", send_general_configuration_values_html);
    MyWebServer.on ( "/admin/reset",     execute_factory_reset_html);
    MyWebServer.on ( "/admin/savequit",     execute_save_quit_html);

    #ifdef Modem_WEB
        MyWebServer.on ( "/modem.html", send_modem_html  );
        MyWebServer.on ( "/admin/modemvalues", send_modem_values_html );
    #else
        MyWebServer.on ( "/modem.html", []() { MyWebServer.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
    #endif

    #ifdef GPS_WEB
        MyWebServer.on ( "/gps.html", send_gps_html  );
        MyWebServer.on ( "/admin/gpsvalues", send_gps_values_html );
    #else
        MyWebServer.on ( "/gps.html", []() { MyWebServer.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
    #endif

    MyWebServer.on ( "/favicon.ico",   []() { MyWebServer.send ( 200, "text/html", "" );   }  );
    MyWebServer.on ( "/style.css", []() { MyWebServer.send ( 200, "text/plain", PAGE_Style_css );  } );
    MyWebServer.on ( "/microajax.js", []() { MyWebServer.send ( 200, "text/plain", PAGE_microajax_js );  } );
    MyWebServer.onNotFound ( []() { MyWebServer.send ( 400, "text/html", "Page not Found" );   }  );
    MyWebServer.begin();
    Extend_time = 900;
    telnet_println("My Web server has started for " + String(Extend_time/60) + " minutes");
  }
  else {
    MyWebServer.close();
    Extend_time = 0;
  }
}

  // WEB commands to run on loop function.
void web_loop() {
    blink_LED(5);
    MyWebServer.handleClient();
    yield();
}
