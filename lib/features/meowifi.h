// Libraries to INCLUDE
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

//meo login captive
const String MEOUSR  = "albkirk@gmail.com";
const String MEOPASS  = "1234qwer";
// the buffer to store the serial input in
byte userInputBuffer[33];
int userBufferI = 0;
byte incoming = 0;


//Initialize HTTPS stack
HTTPClient https;    //Declare object of class HTTPClient

//gerada aqui -> https://www.grc.com/fingerprints.htm
//const String fingerprint  = "50:FE:E2:67:16:26:3C:D0:64:8F:23:43:5D:B2:82:56:A4:5F:03:B0";
//const String fingerprint  = "244229A51F41100E9DD281B5E3FC8A19C73FF949";
//const uint8_t fingerprint[20] = {0x24, 0x42, 0x29, 0xA5, 0x1F, 0x41, 0x10, 0x0E, 0x9D, 0xD2, 0x81, 0xB5, 0xE3, 0xFC, 0x8A, 0x19, 0xC7, 0x3F, 0xF9, 0x49};



void meowifi_https_GET(String URL_MEO) {
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure();
    //client->setFingerprint(fingerprint);

    Serial.println("HTTPS GET...");
    Serial.println(URL_MEO);
    if (https.begin(*client, URL_MEO)) {  // HTTPS

        // start connection and send HTTP header
        int httpCode = https.GET();

        // httpCode will be negative on error
        if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("HTTPS Code: %d\n", httpCode);

            // file found at server
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.print("HTTPS Payload: ");
            Serial.println(payload);
            }
        } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }

        https.end();
    } else {
        Serial.printf("[HTTPS] Unable to connect\n");
    }    
}


void meowifi_login() {
    // Ask the Password for input
    Serial.print("Please enter your Password: ");
    String enc_pwd = "";
    //while(Serial.available() || enc_pwd == "") enc_pwd  = Serial.readString();
    
    // wait for the user input to come
    while (userBufferI < 32 && incoming != 10 && incoming != 13 ) {
      if (Serial.available()) {
        incoming = Serial.read();
        if (incoming != 10 && incoming != 13) {
            userInputBuffer[userBufferI++] = incoming;
            Serial.print((char)incoming);
        }
        else userInputBuffer[userBufferI++] = 0;
      }
    }
    userBufferI = 0;
    userInputBuffer[32] = 0;
    Serial.println("");

    enc_pwd = String((char*)userInputBuffer);
    //Serial.println(enc_pwd);

    String URL_Login = "https://servicoswifi.apps.meo.pt/HotspotConnection.asmx/Login?username=" + MEOUSR + "&password=" + enc_pwd + "&navigatorLang=pt&callback=foo";
    meowifi_https_GET(URL_Login);
}


void meowifi_logout() {
    String URL_Logout = "https://servicoswifi.apps.meo.pt/HotspotConnection.asmx/Logoff?callback=foo";
    meowifi_https_GET(URL_Logout);
}

void meowifi_status() {
    String URL_Status = "https://servicoswifi.apps.meo.pt/HotspotConnection.asmx/GetState?callback=foo&mobile=false&pagePath=foo";
    meowifi_https_GET(URL_Status);
}
