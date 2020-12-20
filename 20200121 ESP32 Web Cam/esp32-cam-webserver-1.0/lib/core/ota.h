#include <ESPmDNS.h>
#include <ArduinoOTA.h>
//#include <custohttpupdate.h>


/*
bool actualUpdate(bool sketch=true){
    String URLString = "http://" + config.UPDATE_Server + ":" + String(config.UPDATE_Port) + "/Firmware/" + BRANDName + "/" + MODELName;
    const char * updateUrl = URLString.c_str();
    String msg;
    t_httpUpdate_return ret;

    ESPhttpUpdate.rebootOnUpdate(false);
    if(sketch){
      Serial.println("Update URL: " + String(updateUrl)); // + "\tUpdate Pass: " + config.UPDATE_Password);
      ret=ESPhttpUpdate.updateup(updateUrl, String(SWVer), config.UPDATE_User, config.UPDATE_Password);
    }
    else {
      ret=ESPhttpUpdate.updateSpiffsup(updateUrl, String(SWVer), config.UPDATE_User, config.UPDATE_Password);
    };
    //Serial.println("Ret Code: " + String(ret));
    if(ret==HTTP_UPDATE_OK){
        telnet_println("HTTP Update SUCCEEDED");
        return true;
    }
    else {
        if(ret==HTTP_UPDATE_NO_UPDATES){
            telnet_println("NO HTTP Update required");
        };
        if(ret==HTTP_UPDATE_FAILED){
            telnet_println("HTTP Update Failed");
        };
    };
    return false;
}
*/

  void ota_setup() {
    // ***********  OTA SETUP
    // Default Port is 8266 for ESP8266 and 3232 for ESP-32
    ArduinoOTA.setPort(8266);

    // Default Hostname is esp8266-[ChipID] for ES8266 and esp3232-[MAC] for ESP-32
    // ArduinoOTA.setHostname("myesp32");

    // No authentication by default
    ArduinoOTA.setPassword((const char *)"12345678");
    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() { // what to do before OTA download insert code here
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()


        telnet_println("Starting OTA update " + type);
      });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onEnd([]() {
        flash_LED(15);      // Flash board led 15 times at end
        telnet_println("\nEnd");
        ESPBoot();
      });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) telnet_println("OTA Auth Failed");
        else if (error == OTA_BEGIN_ERROR) telnet_println("OTA Begin Failed");
        else if (error == OTA_CONNECT_ERROR) telnet_println("OTA Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) telnet_println("OTA Receive Failed");
        else if (error == OTA_END_ERROR) telnet_println("OTA End Failed");
        ESPBoot();
      });

    ArduinoOTA.begin();
    telnet_println("Ready for OTA");
    //if(actualUpdate(true)) ESPBoot();
  }


  // OTA commands to run on loop function.
  void ota_loop() {
      blink_LED(1);
      ArduinoOTA.handle();
      yield();
  }
