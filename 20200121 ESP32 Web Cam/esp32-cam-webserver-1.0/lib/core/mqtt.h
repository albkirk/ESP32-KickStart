// MQTT Constants
#define MQTT_MAX_PACKET_SIZE 512                 // Default: 128 bytes

#include <PubSubClient.h>

// MQTT PATH Structure
// /clientid/location/deviceName/telemetry/<topic>    --> typically, used when publishing info/status
// /clientid/location/deviceName/configure/<topic>    --> typically, used when subscribing for actions

// EXAMPLEs
// /001001/outside/MailBox/telemetry/Status           --> Device status OK / LOWBat
// /001001/room/Estore/telemetry/RSSI                 --> WiFi signal strength value in dBm
// /001001/kitchen/AmbiSense/telemetry/BatLevel       --> Battery Level 0 - 100 %
// /001001/kitchen/AmbSense/configure/DeepSleep       --> Set DeepSleep feature to Enabled - Disabled
// /001001/outside/MailBox/configure/LED              --> Set True / False to turn LED ON/OFF


// MQTT Variables
uint16_t MQTT_state = MQTT_DISCONNECTED;              // MQTT state
uint16_t MQTT_Retry = 125;                            // Timer to retry the MQTT connection
uint16_t MQTT_errors = 0;                             // MQTT errors Counter
uint32_t MQTT_LastTime = 0;                           // Last MQTT connection attempt time stamp

// Initialize MQTT Client
PubSubClient MQTTclient(wifiClient);


// MQTT Functions //
String mqtt_pathtele() {
  return "/" + String(config.ClientID) + "/" + String(config.Location) + "/" + String(config.DeviceName) + "/telemetry/";
}


String mqtt_pathconf() {
  return "/" + String(config.ClientID) + "/" + String(config.Location) + "/" + String(config.DeviceName) + "/configure/";
}


void mqtt_publish(String pubpath, String pubtopic, String pubvalue, boolean toretain = false) {
    String topic = "";
    topic += pubpath; topic += pubtopic;     //topic += "/";
    // Send payload
    if (MQTT_state == MQTT_CONNECTED) {
        if (MQTTclient.publish(topic.c_str(), pubvalue.c_str(), toretain) == 1) telnet_println("MQTT published:  " + String(topic.c_str()) + " = " + String(pubvalue.c_str()));
        else {
            //flash_LED(2);
            telnet_println("");
            telnet_println("!!!!! MQTT message NOT published. !!!!!");
            telnet_println("");
        }
    }
}


void mqtt_subscribe(String subpath, String subtopic) {
    String topic = "";
    topic += subpath; topic += subtopic;
    if (MQTTclient.subscribe(topic.c_str())) telnet_println("subscribed to topic: " + topic);
    else telnet_println("Error on MQTT subscription!");
}


void mqtt_unsubscribe(String subpath, String subtopic) {
    String topic = "";
    topic += subpath; topic += subtopic;
    if( MQTTclient.unsubscribe(topic.c_str())) telnet_println("unsubscribed to topic: " + topic);
    else telnet_println("Error on MQTT unsubscription!");
}


void mqtt_connect() {
    telnet_print("Connecting to MQTT Broker ... ");
    if (WIFI_state != WL_CONNECTED) telnet_println( "ERROR! ==> WiFi NOT Connected!" );
    else {
        MQTTclient.setServer(config.MQTT_Server, config.MQTT_Port);
        // Attempt to connect (clientID, username, password, willTopic, willQoS, willRetain, willMessage, cleanSession)
        if (MQTTclient.connect(ChipID.c_str(), config.MQTT_User, config.MQTT_Password, (mqtt_pathtele() + "Status").c_str(), 0, false, "UShut", true)) {
            MQTT_state = MQTT_CONNECTED;
            telnet_println( "[DONE]" );
            mqtt_subscribe(mqtt_pathconf(), "+");
        }
        else {
            MQTT_state = MQTTclient.state();
            telnet_println("MQTT ERROR! ==> " + String(MQTT_state));
        };
    }
}


void mqtt_disconnect() {
    mqtt_unsubscribe(mqtt_pathconf(), "+");
    MQTTclient.disconnect();
    MQTT_state = MQTT_DISCONNECTED;
    telnet_println("Disconnected from MQTT Broker.");
}


// Adding function with project's customized MQTT actions
#include <custommqtt.h>

// Handling of received message
void on_message(const char* topic, byte* payload, unsigned int length) {

    telnet_println("New message received from Broker");

    char msg[length + 1];
    strncpy (msg, (char*)payload, length);
    msg[length] = '\0';

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
    String reqparam = String((const char*)data["param"]);
    String reqvalue = String((const char*)data["value"]);
    telnet_println("Received Data: " + reqparam + " = " + reqvalue);

    if ( reqparam == "DeviceName") strcpy(config.DeviceName, (const char*)data["value"]);
    if ( reqparam == "Location") strcpy(config.Location, (const char*)data["value"]);
    if ( reqparam == "ClientID") strcpy(config.ClientID, (const char*)data["value"]);
    if ( reqparam == "DEEPSLEEP") { config.DEEPSLEEP = bool(data["value"]);storage_write(); }
    if ( reqparam == "SLEEPTime") { config.SLEEPTime = data["value"];storage_write(); }
    if ( reqparam == "ONTime") { config.ONTime = data["value"];storage_write(); }
    if ( reqparam == "ExtendONTime") if (bool(data["value"]) == true) Extend_time = 60;
    if ( reqparam == "LED") config.LED = bool(data["value"]);
    if ( reqparam == "TELNET") { config.TELNET = bool(data["value"]); storage_write(); ESPBoot(); }
    if ( reqparam == "OTA") { config.OTA = bool(data["value"]); storage_write(); ESPBoot(); }
    if ( reqparam == "WEB") { config.WEB = bool(data["value"]); storage_write(); ESPBoot(); }
    if ( reqparam == "STAMode") config.STAMode = bool(data["value"]);
    if ( reqparam == "ssid") strcpy(config.ssid, (const char*)data["value"]);
    if ( reqparam == "WiFiKey") strcpy(config.WiFiKey, (const char*)data["value"]);
    if ( reqparam == "NTPServerName") strcpy(config.NTPServerName, (const char*)data["value"]);
    if ( reqparam == "Update_Time_Via_NTP_Every") config.Update_Time_Via_NTP_Every = data["value"];
    if ( reqparam == "TimeZone") config.TimeZone = data["value"];
    if ( reqparam == "isDayLightSaving") config.isDayLightSaving = bool(data["value"]);
    if ( reqparam == "Store") if (bool(data["value"]) == true) storage_write();
    if ( reqparam == "Boot") if (bool(data["value"]) == true) ESPBoot();
    if ( reqparam == "Reset") if (bool(data["value"]) == true) storage_reset();
    if ( reqparam == "Temp_Corr") {
		    config.Temp_Corr = data["value"];
		    storage_write();
		    mqtt_publish(mqtt_pathtele(), "Temperatura", String(getTemperature()));
	  }
    mqtt_custom(reqparam, reqvalue, data);
    storage_print();
}


// The callback for when a PUBLISH message is received from the server.
void mqtt_callback() {
    MQTTclient.setCallback(on_message);
}


// MQTT commands to run on setup function.
void mqtt_setup() {
    mqtt_connect();
    mqtt_callback();
    if (MQTT_state == MQTT_CONNECTED) {
        if (String(rtc_get_wakeup_cause()) != "DEEPSLEEP_RESET") {
            mqtt_publish(mqtt_pathtele(), "Boot", String(rtc_get_wakeup_cause()));
            mqtt_publish(mqtt_pathtele(), "ChipID", ChipID);
            mqtt_publish(mqtt_pathtele(), "Brand", BRANDName);
            mqtt_publish(mqtt_pathtele(), "Model", MODELName);
            mqtt_publish(mqtt_pathtele(), "SWVer", SWVer);
        }
        if (BattPowered) {
            // Check Battery Level
            Batt_Level = getVoltage();
            mqtt_publish(mqtt_pathtele(), "BatLevel", String(Batt_Level));
            if (Batt_Level > Batt_L_Thrs) mqtt_publish(mqtt_pathtele(), "Status", "Battery");
            else mqtt_publish(mqtt_pathtele(), "Status", "LOW Battery");
        }
        else mqtt_publish(mqtt_pathtele(), "Status", "Mains");
        mqtt_publish(mqtt_pathtele(), "RSSI", String(getRSSI()));
        mqtt_publish(mqtt_pathtele(), "IP", WiFi.localIP().toString());
    }
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
