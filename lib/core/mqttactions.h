// Adding function with project's customized MQTT actions
#include <custommqtt.h>

// Handling of received message
void on_message(const char* topic, byte* payload, unsigned int msg_length) {

    telnet_println("New message received from Broker");

    char msg[msg_length + 1];
    strncpy (msg, (char*)payload, msg_length);
    msg[msg_length] = '\0';

    telnet_println("Topic: " + String(topic));
    telnet_println("Payload: " + String((char*)msg));

    // Decode JSON request
    StaticJsonDocument<200> data;
    DeserializationError JSONerror = deserializeJson(data, msg);

    if (JSONerror) {
      telnet_println("JSON deserialization failed!. Error code: " + String(JSONerror.c_str()));
      return;
    }

    // Check request method
    String reqtopic = String(topic);
    String reqparam = String((const char*)data["param"]);
    String reqvalue = String((const char*)data["value"]);
    if (data["value"].is<const char*>() == false) reqvalue = String((long)data["value"]);
    telnet_println("Received Data: " + reqparam + " = " + reqvalue);


    // System Configuration 
    if ( reqparam == "DeviceName") strcpy(config.DeviceName, (const char*)data["value"]);
    if ( reqparam == "Location") strcpy(config.Location, (const char*)data["value"]);
    if ( reqparam == "ClientID") strcpy(config.ClientID, (const char*)data["value"]);
    if ( reqparam == "DEEPSLEEP") { config.DEEPSLEEP = bool(data["value"]);storage_write(); }
    if ( reqparam == "SLEEPTime") { config.SLEEPTime = data["value"];storage_write(); }
    if ( reqparam == "ONTime") { config.ONTime = data["value"];storage_write(); }
    if ( reqparam == "ExtendONTime") if (bool(data["value"]) == true) Extend_time = 60;
    if ( reqparam == "LED") config.LED = bool(data["value"]);
    if ( reqparam == "TELNET") { config.TELNET = bool(data["value"]); storage_write(); mqtt_restart(); }
    if ( reqparam == "OTA") { config.OTA = bool(data["value"]); storage_write(); mqtt_restart(); }
    if ( reqparam == "WEB") { config.WEB = bool(data["value"]); storage_write(); mqtt_restart(); }
    if ( reqparam == "DHCP") { config.DHCP = bool(data["value"]); storage_write(); mqtt_restart(); }
    if ( reqparam == "STAMode") config.STAMode = bool(data["value"]);
    if ( reqparam == "ssid") strcpy(config.ssid, (const char*)data["value"]);
    if ( reqparam == "WiFiKey") strcpy(config.WiFiKey, (const char*)data["value"]);
    if ( reqparam == "NTPServerName") strcpy(config.NTPServerName, (const char*)data["value"]);
    if ( reqparam == "Update_Time_Via_NTP_Every") config.Update_Time_Via_NTP_Every = data["value"];
    if ( reqparam == "TimeZone") config.TimeZone = data["value"];
    if ( reqparam == "isDayLightSaving") config.isDayLightSaving = bool(data["value"]);
    if ( reqparam == "Store") if (bool(data["value"]) == true) storage_write();
    if ( reqparam == "Boot") if (bool(data["value"]) == true) mqtt_restart();
    if ( reqparam == "Reset") if (bool(data["value"]) == true) storage_reset();
    if ( reqparam == "Switch_Def") { 
            config.SWITCH_Default = bool(data["value"]);
            storage_write();
            mqtt_publish(mqtt_pathtele(), "Switch", String(SWITCH));
       }
    if ( reqparam == "Temp_Corr") { 
            config.Temp_Corr = float(data["value"]);
            storage_write();
            mqtt_publish(mqtt_pathtele(), "Temperatura", String(getTemperature()));
       }
    if ( reqparam == "LDO_Corr") { 
            config.LDO_Corr = float(data["value"]);
            storage_write();
            mqtt_publish(mqtt_pathtele(), "BattLevel", String(getVoltage()));
       }

    // Standard Actuators/Actions 
    if ( reqparam == "Level") LEVEL = uint(data["value"]);
    if ( reqparam == "Position") POSITION = int(data["value"]);
    if ( reqparam == "Switch") SWITCH = bool(data["value"]);
    if ( reqparam == "Timer") TIMER = ulong(data["value"]);
    if ( reqparam == "Counter") COUNTER = ulong(data["value"]);
    if ( reqparam == "Calibrate") { CALIBRATE = float(data["value"]); }


    mqtt_custom(reqtopic, reqparam, data);

    storage_print();
}


// The callback to handle the MQTT PUBLISH messages received from Broker.
void mqtt_setcallback() {
    MQTTclient.setCallback(on_message);
}


// MQTT commands to run on setup function.
void mqtt_setup() {
    mqtt_connect();
    if (MQTT_state == MQTT_CONNECTED) {
        if (ESPWakeUpReason() != "Deep-Sleep Wake") {
            mqtt_publish(mqtt_pathtele(), "Boot", ESPWakeUpReason());
            mqtt_publish(mqtt_pathtele(), "Brand", BRANDName);
            mqtt_publish(mqtt_pathtele(), "Model", MODELName);
            mqtt_publish(mqtt_pathtele(), "ChipID", ChipID);
            mqtt_publish(mqtt_pathtele(), "SWVer", SWVer);
        }
        status_report();
        mqtt_publish(mqtt_pathtele(), "RSSI", String(getRSSI()));
        mqtt_publish(mqtt_pathtele(), "IP", WiFi.localIP().toString());
    }
    mqtt_setcallback();
}


// MQTT commands to run on loop function.
void mqtt_loop() {
    if (!MQTTclient.loop()) {
        if ( millis() - MQTT_LastTime > (MQTT_Retry * 1000)) {
            MQTT_errors ++;
            Serial.print( "in loop function MQTT ERROR! #: " + String(MQTT_errors) + "  ==> "); Serial.println( MQTTclient.state() );
            MQTT_LastTime = millis();
            mqtt_connect();
        }
    }
    yield();
}