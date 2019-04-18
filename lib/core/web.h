#include <ESPmDNS.h>
#include <WebServer.h>

ESP8266WebServer MyWebServer(80);  // The Webserver

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

void ConfigureWifi(){
  Serial.println("Configuring Wifi");
  WiFi.begin (config.ssid, config.WiFiKey);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    delay(500);
  }
  if (!config.dhcp)
  {
    WiFi.config(IPAddress(config.IP[0], config.IP[1], config.IP[2], config.IP[3] ),  IPAddress(config.Gateway[0], config.Gateway[1], config.Gateway[2], config.Gateway[3] ) , IPAddress(config.Netmask[0], config.Netmask[1], config.Netmask[2], config.Netmask[3] ));
  }
}


// MAIN Functions

	// Include the HTML, STYLE and Script "Pages"
	#include "Page_Admin.h"
	#include "Page_General.h"
	#include "Page_Information.h"
	#include "Page_NetworkConfiguration.h"
	#include "Page_NTPSettings.h"
	#include "Page_FactoryReset.h"
	#include "Page_Script.js.h"
	#include "Page_Style.css.h"

void web_setup() {
    // Start HTTP Server for configuration
    MyWebServer.on ( "/", []() {
      Serial.println("admin.html");
      MyWebServer.send_P ( 200, "text/html", PAGE_AdminMainPage);  // const char top of page
    }  );

    MyWebServer.on ( "/favicon.ico",   []() {
      Serial.println("favicon.ico");
      MyWebServer.send( 200, "text/html", "" );
    }  );

    // Example Page
    //  MyWebServer.on ( "/example.html", []() { MyWebServer.send_P ( 200, "text/html", PAGE_EXAMPLE );  } );

    // Info Page
    MyWebServer.on ( "/info.html", []() {
      Serial.println("info.html");
      MyWebServer.send_P ( 200, "text/html", PAGE_Information );
    }  );

	// General info Page
    MyWebServer.on ( "/general.html", send_general_html );

    // Network config
    MyWebServer.on ( "/config.html", send_network_configuration_html );

	// NTP config
    MyWebServer.on ( "/ntp.html", send_NTP_configuration_html  );

	// Factory Defaults Page
    MyWebServer.on ( "/reset.html", factory_reset_html  );

	// HTTP style config
    MyWebServer.on ( "/style.css", []() {
      Serial.println("style.css");
      MyWebServer.send_P ( 200, "text/plain", PAGE_Style_css );
    } );

	// HTTP microaxaj code
    MyWebServer.on ( "/microajax.js", []() {
      Serial.println("microajax.js");
      MyWebServer.send_P ( 200, "text/plain", PAGE_microajax_js );
    } );

    MyWebServer.on ( "/admin/devicename", send_devicename_value_html);
    MyWebServer.on ( "/admin/generalvalues", send_general_configuration_values_html);
    MyWebServer.on ( "/admin/values", send_network_configuration_values_html );
    MyWebServer.on ( "/admin/connectionstate", send_connection_state_values_html );
    MyWebServer.on ( "/admin/infovalues", send_information_values_html );
    MyWebServer.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
	MyWebServer.on ( "/admin/reset", execute_factory_reset_html);

    MyWebServer.onNotFound ( []() {
      Serial.println("Page Not Found");
      MyWebServer.send ( 400, "text/html", "Page not Found" );
    }  );


    MyWebServer.begin();
    Serial.println( "My Web server has started" );
}

  // WEB commands to run on loop function.
void web_loop() {
    blink_LED(5);
    MyWebServer.handleClient();
    yield();
}
