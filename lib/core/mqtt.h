// MQTT Constants
#define MQTT_MAX_PACKET_SIZE 512                 // Default: 128 bytes

#include <PubSubClient.h>

// MQTT PATH Structure
// /clientid/location/deviceName/telemetry/<topic>    --> typically, used when publishing info/status
// /clientid/location/deviceName/configure/<topic>    --> typically, used when subscribing for actions

// EXAMPLEs
// /001001/outside/MailBox/telemetry/Status           --> Device status OK / LOWBat
// /001001/room/Estore/telemetry/RSSI                 --> WiFi signal strength value in dBm
// /001001/kitchen/AmbiSense/telemetry/BattLevel       --> Battery Level 0 - 100 %
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

void mqtt_restart() {
    mqtt_publish(mqtt_pathtele(), "Status", "Restarting");
    mqtt_disconnect();
    ESPRestart();
}
