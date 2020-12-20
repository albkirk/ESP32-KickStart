// Adding function with project's customized MQTT actions
#include <customactions.h>

void state_update() {
            //mqtt_publish(mqtt_pathtele, "Boot", ESPWakeUpReason());
            //mqtt_publish(mqtt_pathtele, "Brand", BRANDName);
            //mqtt_publish(mqtt_pathtele, "Model", MODELName);
            //mqtt_publish(mqtt_pathtele, "ChipID", ChipID);
            //mqtt_publish(mqtt_pathtele, "SWVer", SWVer);
            status_report();                            // should send Status MAIN or Battery
            //mqtt_publish(mqtt_pathtele, "IP", WiFi.localIP().toString());
            if (LED_ESP>=0) mqtt_publish(mqtt_pathtele, "LED", String(config.LED));
            mqtt_publish(mqtt_pathtele, "RSSI", String(getRSSI()));
            hassio_attributes();
            custom_update();
}


// Handling of received message
void on_message(const char* topic, byte* payload, unsigned int msg_length) {

    telnet_println("New message received from Broker");

    char msg[msg_length + 1];
    strncpy (msg, (char*)payload, msg_length);
    msg[msg_length] = '\0';

    //telnet_println("Topic: " + String(topic));
    //telnet_println("Payload: " + String((char*)msg));

    // Check requested command
    String command = String(topic);
    command.replace(mqtt_pathsubs, "");
    String cmd_value = String((char*)msg);
    telnet_print("Requested Command: " + command);
    telnet_println("\tCommand Value: " + cmd_value);

    // System Configuration 
    if ( command == "DeviceName") {hassio_delete(); strcpy(config.DeviceName, cmd_value.c_str()); hassio_discovery(); hassio_attributes(); storage_write(); }
    if ( command == "Location") {strcpy(config.Location, cmd_value.c_str()); config_backup(); hassio_attributes(); storage_write(); }
    if ( command == "ClientID") {hassio_delete(); strcpy(config.ClientID, cmd_value.c_str()); hassio_discovery(); hassio_attributes(); storage_write(); }
    if ( command == "DEEPSLEEP") { config.DEEPSLEEP = bool(cmd_value.toInt());storage_write(); }
    if ( command == "SLEEPTime") { config.SLEEPTime = byte(cmd_value.toInt());storage_write(); }
    if ( command == "ONTime") { config.ONTime = byte(cmd_value.toInt());storage_write(); }
    if ( command == "ExtendONTime") if (bool(cmd_value.toInt()) == true) Extend_time = 60;
    if ( command == "LED") {config.LED = bool(cmd_value.toInt()); mqtt_publish(mqtt_pathtele, "LED", String(config.LED));}
    if ( command == "TELNET") { config.TELNET = bool(cmd_value.toInt()); storage_write(); telnet_setup(); }
    if ( command == "OTA") { config.OTA = bool(cmd_value.toInt()); storage_write(); ESPRestart(); }
    if ( command == "NTP") if (bool(cmd_value.toInt()) == true) { getNTPtime(); mqtt_publish(mqtt_pathtele, "DateTime", String(curDateTime()));}
    if ( command == "WEB") { config.WEB = bool(cmd_value.toInt()); storage_write(); web_setup(); }
    if ( command == "DHCP") { config.DHCP = bool(cmd_value.toInt()); storage_write(); wifi_connect(); }
    if ( command == "STAMode") { config.STAMode = bool(cmd_value.toInt()); storage_write(); wifi_connect(); }
    if ( command == "APMode") { config.APMode = bool(cmd_value.toInt()); storage_write(); wifi_connect(); }
    if ( command == "SSID") strcpy(config.SSID, cmd_value.c_str());
    if ( command == "WiFiKey") strcpy(config.WiFiKey, cmd_value.c_str());
    if ( command == "NTPServerName") strcpy(config.NTPServerName, cmd_value.c_str());
    if ( command == "MQTT_Server") { strcpy(config.MQTT_Server, cmd_value.c_str()); storage_write(); }
    if ( command == "MQTT_Port") { config.MQTT_Port = (long)cmd_value.toInt();storage_write(); }
    if ( command == "MQTT_Secure") { config.MQTT_Secure = bool(cmd_value.toInt()); storage_write(); }
    if ( command == "MQTT_User") { strcpy(config.MQTT_User, cmd_value.c_str()); storage_write(); }
    if ( command == "MQTT_Password") { strcpy(config.MQTT_Password, cmd_value.c_str()); storage_write(); }   
    if ( command == "SIMCardPIN") { strcpy(config.SIMCardPIN, cmd_value.c_str()); storage_write(); }
    if ( command == "APN") { strcpy(config.APN, cmd_value.c_str()); storage_write(); }
    if ( command == "MODEM_User") { strcpy(config.MODEM_User, cmd_value.c_str()); storage_write(); }
    if ( command == "MODEM_Password") { strcpy(config.MODEM_Password, cmd_value.c_str()); storage_write(); }   
    if ( command == "Update_Time_Via_NTP_Every") config.Update_Time_Via_NTP_Every = (ulong)abs(atol(cmd_value.c_str()));
    if ( command == "TimeZone") config.TimeZone = (long)cmd_value.toInt();
    if ( command == "isDayLightSaving") config.isDayLightSaving = bool(cmd_value.toInt());
    if ( command == "DEBUG") { config.DEBUG = bool(cmd_value.toInt()); telnet_println("DEBUG = " + String(config.DEBUG)); storage_write(); }
    if ( command == "Store") if (bool(cmd_value.toInt()) == true) storage_write();
    if ( command == "Boot")  if (bool(cmd_value.toInt()) == true) {mqtt_publish(mqtt_pathcomd, "Boot", "", true); mqtt_restart();}
    if ( command == "Reset") if (bool(cmd_value.toInt()) == true) {mqtt_publish(mqtt_pathcomd, "Reset", "", true); hassio_delete(); mqtt_reset();}
    if ( command == "HASSIO") if (bool(cmd_value.toInt()) == true) {
            mqtt_publish(mqtt_pathcomd, "HASSIO", "", true);
            hassio_delete();
            hassio_discovery();
            delay(10);
            state_update();
        }
    if ( command == "Switch_Def") { 
            config.SWITCH_Default = bool(cmd_value.toInt());
            storage_write();
            mqtt_publish(mqtt_pathtele, "Switch", String(SWITCH));
       }
    if ( command == "Temp_Corr") { 
            config.Temp_Corr = cmd_value.toFloat();
            storage_write();
            mqtt_publish(mqtt_pathtele, "Temp_Corr", String(config.Temp_Corr));
       }
    if ( command == "LDO_Corr") { 
            config.LDO_Corr = cmd_value.toFloat();
            storage_write();
            mqtt_publish(mqtt_pathtele, "Battery", String(getBattLevel(),0));
       }

    // Standard Actuators/Actions 
    if ( command == "Level") LEVEL = (uint)abs(cmd_value.toInt());
    if ( command == "Position") POSITION = cmd_value.toInt();
    if ( command == "Switch") {
        if ( SWITCH_Last == bool(cmd_value.toInt()) ) mqtt_publish(mqtt_pathtele, "Switch", String(SWITCH));
        else SWITCH = bool(cmd_value.toInt());
    }
    if ( command == "Timer") TIMER = (ulong)abs(atol(cmd_value.c_str()));
    if ( command == "Counter") COUNTER = (ulong)abs(atol(cmd_value.c_str()));
    if ( command == "Calibrate") { CALIBRATE = cmd_value.toFloat(); }

    custom_mqtt(command, cmd_value);

    if (config.DEBUG) {
        storage_print();
        Serial.print("Current Local Date / Time: " + curDateTime());
        Serial.printf("\t NTP Sync: %d\n", NTP_Sync);
    }
}


// The callback to handle the MQTT PUBLISH messages received from Broker.
void mqtt_setcallback() {
    MQTTclient.setCallback(on_message);
}


// MQTT commands to run on setup function.
void mqtt_setup() {
    mqtt_pathtele = String(config.ClientID) + "/" + String(ChipID) + "/" + String(config.DeviceName) + "/inform/";
    mqtt_pathcomd = String(config.ClientID) + "/" + String(ChipID) + "/" + String(config.DeviceName) + "/command/";
    mqtt_pathconf = String(config.ClientID) + "/" + String(ChipID) + "/" + String(config.DeviceName) + "/config/";
    mqtt_pathsubs = mqtt_pathcomd;

    mqtt_set_client();
    mqtt_connect();
    mqtt_setcallback();
    if (MQTT_state == MQTT_CONNECTED) {
        if (ESPWakeUpReason() == "Deep-Sleep Wake") {
            mqtt_publish(mqtt_pathtele, "Status", "WakeUp");
            mqtt_publish(mqtt_pathtele, "Battery", String(getBattLevel(),0));
        }
        else {
            // 1st RUN ?
            //if(Load_Default) yield();                 // NOTE! this var drops after boot/sleep, regardless of Wifi/MQTT connection success.
            if(config.SW_Upgraded) {
                config.HASSIO_CFG = false;              // this forces HASSIO Discovery device info to be updated
                config.SW_Upgraded = false;             // The house is cleaned... clearing the flag.
                storage_write();
            }
            // HASSIO Discovery configured?
            if(!config.HASSIO_CFG) {
                if (bckup_rstr_flag) mqtt_restore();
                hassio_discovery();
            }
            trigger_syncme();
            state_update();
        }
    }
}

// MQTT commands to run on loop function.
void mqtt_loop() {
    if (!MQTTclient.loop()) {
        if ( millis() - MQTT_LastTime > (MQTT_Retry * 1000)) {
            MQTT_errors ++;
            Serial.println( "in loop function MQTT ERROR! #: " + String(MQTT_errors) + "  ==> " + MQTT_state_string(MQTTclient.state()) );
            MQTT_LastTime = millis();
            mqtt_connect();
            if( MQTT_state == MQTT_CONNECTED) state_update();
        }
    }
    yield();
}


void parse_command_msg(String bufferRead) {
        char msg_array[bufferRead.length()+1];
        strcpy(msg_array, bufferRead.c_str());
        msg_array[bufferRead.length()] = 0;

        if((msg_array[0]=='A' || msg_array[0]=='a') && (msg_array[1] == 'T' || msg_array[1] == 't')) {
            if (config.DEBUG) Serial.println(bufferRead);
            parse_at_command(bufferRead);
        }
        else {
            char * equalPosition = strchr(msg_array,'=');
            //Serial.println(equalPosition);
            if(equalPosition > 0) {
                char * pch;
                pch = strtok (msg_array," ,-=\"");
                String command = String((char *)pch);
                pch = strtok (NULL, " ,-=\"");
                String value = String((char *)pch);
                //Serial.print(command); Serial.print("\t<-->\t"); Serial.println(value);

                byte B_value[value.length()+1];
                value.getBytes(B_value, value.length()+1);
                //B_value[value.length()+1] = 0;

                on_message(command.c_str(), B_value, value.length()+1);
            }
        }
        TELNET_Timer = millis();                // Update timer to extend telnet inactivity timeout
}


// TELNET commands to run on loop function.
void telnet_loop() {
		blink_LED(3);
	   //check if there are any new clients
    if (telnetClient.connected()) {
        if ((millis() - TELNET_Timer) > MAX_TIME_INACTIVE) {
            telnetClient.println("Closing Telnet session by inactivity");
            telnetClient.stop();
        }
    }

    if (telnetServer.hasClient()) {
        if (telnetClient && telnetClient.connected()) {
            // Verify if the IP is same than actual conection
            newClient = telnetServer.available();
            if (newClient.remoteIP() == telnetClient.remoteIP() ) {
                // Reconnect
                telnetClient.stop();
                telnetClient = newClient;
            }
            else {
                // Disconnect (not allow more than one connection)
                newClient.stop();
                return;
            }
        }
        else {
            // New TCP client
            telnetClient = telnetServer.available();
            telnetClient.setNoDelay(true);      // Faster... ?
            telnetClient.flush();               // clear input buffer, else you get strange characters
            TELNET_Timer = millis();            // initiate timer for inactivity
            if (config.DEBUG) Serial.println("New telnet client!");
        }
    }

    if (telnetClient.available()) parse_command_msg(telnetClient.readStringUntil('\n'));

    yield();
}
