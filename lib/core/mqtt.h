// MQTT Constants
#define MQTT_new_MAX_PACKET_SIZE 512                // Default: 256 bytes
#define MQTT_new_KEEPALIVE 60                       // Default: 15 seconds
#define MQTT_new_SOCKET_TIMEOUT 3                   // Default: 15 seconds
#include <PubSubClient.h>


// MQTT PATH Structure
// /ClientID/Location/DeviceName/telemetry/<Param>  --> typically, used when publishing info/status
// /ClientID/Location/DeviceName/configure/<Param>  --> typically, used when subscribing for actions

// EXAMPLEs
// TOPIC                                           MESSAGE
// /001001/Outside/MailBox/telemetry/Status        "Mains"                             --> Device status OK / LOWBat
// /001001/Room/Estore/telemetry/RSSI              -89                                 --> WiFi signal strength value in dBm
// /001001/kitchen/AmbiSense/telemetry/BattLevel   33                                  --> Battery Level 0 - 100 %
// /001001/kitchen/AmbSense/configure/DeepSleep    {command:"DeepSleep", value:false}  --> Set DeepSleep feature to Enabled - Disabled
// /001001/Outside/MailBox/configure/LED           {command:"LED", value:false}        --> Set True / False to turn LED ON/OFF


// MQTT Variables
static const String MQTT_state_Name[] = {
    "MQTT_CONNECTION_TIMEOUT",      // -4
    "MQTT_CONNECTION_LOST",         // -3
    "MQTT_CONNECT_FAILED",          // -2
    "MQTT_DISCONNECTED",            // -1
    "MQTT_CONNECTED",               // 0
    "MQTT_CONNECT_BAD_PROTOCOL",    // 1
    "MQTT_CONNECT_BAD_CLIENT_ID",   // 2
    "MQTT_CONNECT_UNAVAILABLE",     // 3
    "MQTT_CONNECT_BAD_CREDENTIALS", // 4
    "MQTT_CONNECT_UNAUTHORIZED"     // 5
};

int16_t MQTT_state = MQTT_DISCONNECTED;             // MQTT state
uint16_t MQTT_Retry = 125;                          // Timer to retry the MQTT connection
uint16_t MQTT_errors = 0;                           // MQTT errors Counter
uint32_t MQTT_LastTime = 0;                         // Last MQTT connection attempt time stamp
static String mqtt_pathtele = "";                   // Topic path for publish information
static String mqtt_pathcomd = "";                   // Topic path for receiving commands
static String mqtt_pathconf = "";                   // Topic path for Backup/Restore data (JSON string)
static String mqtt_pathsubs = "";                   // Topic path for subscription

// Backup/Restore
bool bckup_rstr_flag = true;                        // Enable this flag if there's data to backup restore
unsigned long rstr_syn_timeout= 200UL;              // time out limit to wait for the restore/syncMe MQTT packets 
DynamicJsonDocument config_doc(256);                // JSON entity for configuration Backup/Restore (via MQTT)
char config_jsonString[256];                        // Correspondent string variable

PubSubClient MQTTclient;                            // The MQTT Client instance

// MQTT Functions //
String  MQTT_state_string(int mqttstate = MQTT_state){
   return MQTT_state_Name[map(mqttstate, -4, 5, 0 , 9)];
}


bool mqtt_publish(String pubpath, String pubtopic, String pubvalue, boolean toretain = false) {
    String topic = "";
    topic += pubpath; topic += pubtopic;
    // Send payload
    if (MQTTclient.state() == MQTT_CONNECTED) {
        if (MQTTclient.publish(topic.c_str(), pubvalue.c_str(), toretain) == 1) {
            telnet_println("MQTT published:  " + String(topic.c_str()) + " = " + String(pubvalue.c_str()));
            return true;
        }
        else {
            //flash_LED(2);
            telnet_println("");
            telnet_println("!!!!! MQTT message NOT published. !!!!!");
            telnet_println("");
        }
        yield();                                // Required, or else it won't publish messages in burst
    }
    MQTT_state = MQTTclient.state();
    return false;
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

void mqtt_set_client() {
        if (config.MQTT_Secure) MQTTclient.setClient(secureclient);
        else MQTTclient.setClient(unsecuclient);
}

void mqtt_connect(String Will_Topic = (mqtt_pathtele + "Status"), String Will_Msg = "UShut") {
    if (WIFI_state != WL_CONNECTED && !Celular_Connected) telnet_println( "MQTT ERROR! ==> NO Internet connection!" );
    else if (config.MQTT_Secure && !NTP_Sync) telnet_println( "MQTT ERROR! ==> NTP Required but NOT Sync!" );
    else {
        telnet_print("Connecting to MQTT Broker ... ");
        MQTTclient.setBufferSize(MQTT_new_MAX_PACKET_SIZE);
        MQTTclient.setKeepAlive(MQTT_new_KEEPALIVE);
        MQTTclient.setSocketTimeout(MQTT_new_SOCKET_TIMEOUT);
        MQTTclient.setServer(config.MQTT_Server, config.MQTT_Port);
        // Attempt to connect (clientID, username, password, willTopic, willQoS, willRetain, willMessage, cleanSession)
        if (MQTTclient.connect(ChipID.c_str(), config.MQTT_User, config.MQTT_Password, Will_Topic.c_str(), 0, false, Will_Msg.c_str(), true)) {
            MQTT_state = MQTT_CONNECTED;
            telnet_println( "[DONE]" );
            mqtt_subscribe(mqtt_pathcomd, "+");
        }
        else {
            MQTT_state = MQTTclient.state();
            telnet_println("MQTT ERROR! ==> " + MQTT_state_string(MQTT_state));
        };
    }
}


void mqtt_disconnect() {
    mqtt_unsubscribe(mqtt_pathcomd, "+");
    MQTTclient.disconnect();
    MQTT_state = MQTT_DISCONNECTED;
    telnet_println("Disconnected from MQTT Broker.");
}


void mqtt_restart() {
    mqtt_publish(mqtt_pathtele, "Status", "Restarting");
    mqtt_disconnect();
    ESPRestart();
}


void mqtt_reset() {
    mqtt_publish(mqtt_pathtele, "Status", "Reseting");
    mqtt_disconnect();
    storage_reset();
    RTC_reset();
    ESPRestart();
}


void mqtt_restore() {
    unsigned long start_time = millis();
    mqtt_pathsubs = mqtt_pathconf;
    mqtt_subscribe(mqtt_pathconf, "BckpRstr");
    while (bckup_rstr_flag && millis() - start_time < rstr_syn_timeout )
    {
        MQTTclient.loop();
    }
    
    mqtt_unsubscribe(mqtt_pathconf, "BckpRstr");
    mqtt_pathsubs = mqtt_pathcomd;

}
