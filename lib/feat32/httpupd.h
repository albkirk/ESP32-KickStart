#include <custohttpupdate.h>


bool HTTPUpdate(bool sketch=true){
    String URLString = "http://" + String(config.UPDATE_Server) + ":" + String(config.UPDATE_Port) + "/Firmware/" + BRANDName + "/" + MODELName;
    const char * updateUrl = URLString.c_str();
    String msg;
    t_httpUpdate_return ret;

    ESPhttpUpdate.rebootOnUpdate(false);
    if(sketch){
      if (config.DEBUG) Serial.println("Update URL: " + String(updateUrl)); // + "\tUpdate Pass: " + config.UPDATE_Password);
      ret=ESPhttpUpdate.updateup(updateUrl, String(SWVer), config.UPDATE_User, config.UPDATE_Password);
    }
    else {
      ret=ESPhttpUpdate.updateSpiffsup(updateUrl, String(SWVer), config.UPDATE_User, config.UPDATE_Password);
    };
    //Serial.println("Ret Code: " + String(ret));
    if(ret==HTTP_UPDATE_OK){
        if (config.DEBUG) Serial.println("HTTP Update SUCCEEDED");
        return true;
    }
    else {
        if(ret==HTTP_UPDATE_NO_UPDATES){
            if (config.DEBUG) Serial.println("NO HTTP Update required");
        };
        if(ret==HTTP_UPDATE_FAILED){
            if (config.DEBUG) Serial.println("HTTP Update Failed");
        };
    };
    return false;
}

void http_upg() {
    if (WIFI_state != WL_CONNECTED) { if (config.DEBUG) Serial.println( "UPG ERROR! ==> WiFi NOT Connected!" );}
    else if(HTTPUpdate(true)) {
        config.SW_Upgraded = true;
        storage_write();
        ESPRestart();
    }
}
