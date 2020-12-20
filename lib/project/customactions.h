// Function to insert customized MQTT Configuration actions
void custom_mqtt(String command, String cmd_value) {
  if ( command == "Info" && bool(cmd_value.toInt())) {
    //status_report();
    if (BattPowered) mqtt_publish(mqtt_pathtele, "Battery", String(getBattLevel(),0));
    mqtt_publish(mqtt_pathtele, "NTP_Sync", String(NTP_Sync));
    mqtt_publish(mqtt_pathtele, "DateTime", String(curDateTime()));
    hassio_attributes();
    //ambient_data();
    telnet_println("WiFi State: " + WIFI_state_Name[WIFI_state] + "\t MQTT State: " + MQTT_state_string());
    telnet_println("Modem State: " + Modem_state_Name[Modem_state] + "\t REG State: " + RegStatus_string(modem.getRegistrationStatus()));
  }
  if ( command == "Config" && bool(cmd_value.toInt())) {
    mqtt_publish(mqtt_pathtele, "OTA", String(config.OTA));
    mqtt_publish(mqtt_pathtele, "TELNET", String(config.TELNET));
    mqtt_publish(mqtt_pathtele, "WEB", String(config.WEB));
    mqtt_publish(mqtt_pathtele, "Interval", String(config.SLEEPTime));   
    mqtt_publish(mqtt_pathtele, "Temp_Corr", String(config.Temp_Corr));
    mqtt_publish(mqtt_pathtele, "LDO_Corr", String(config.LDO_Corr));
    config_backup();
  }
  
  if ( command == "MODEM_ON" && bool(cmd_value.toInt())) MODEM_ON();
  if ( command == "MODEM_OFF" && bool(cmd_value.toInt())) MODEM_OFF();
  if ( command == "MODEM_Connect" && bool(cmd_value.toInt())) MODEM_Connect();
  if ( command == "MODEM_Disconnect" && bool(cmd_value.toInt())) MODEM_Disconnect();
  if ( command == "MODEM_Info" && bool(cmd_value.toInt())) MODEM_Info();
  if ( command == "send_Telemetry" && bool(cmd_value.toInt())) { gps_update(); print_gps_data(); send_Telemetry(); }
  if ( command == "isconnected" && bool(cmd_value.toInt())) Serial.println(modem.isNetworkConnected());

  if ( command == "BattPowered") BattPowered = bool(cmd_value.toInt());
  if ( command == "GPS_Switch") {if (bool(cmd_value.toInt())) { gps_start(); print_gps_data ();} else gps_stop();}
  
  if ( command == "BckpRstr") {
      if (cmd_value == "") telnet_println("Restore data is empty");
      else {
          DeserializationError error = deserializeJson(config_doc, cmd_value); //Deserialize string to JSON data
          if (error) {telnet_print("JSON DeSerialization failure: "); telnet_println(error.f_str());}
          else {

                strcpy(config.Location, config_doc["Location"]);
/*
                config.LOWER_LEVEL =    config_doc["LOWER_Pos"];
                config.UPPER_LEVEL =    config_doc["UPPER_Pos"];
*/
                storage_write();
                hassio_attributes();
                bckup_rstr_flag = false;
          }
      }
  }
}

void custom_update(){
    yield();
//    mqtt_dump_data(mqtt_pathtele, "Telemetry");
}



// TELNET related actions


void parse_at_command(String msg) {
    #ifdef Modem
        msg.remove(0, 2);       // to remove "AT"
        modem.sendAT(msg);
        String res = "";
        unsigned long timeout = millis();
        while ( (millis() - timeout < 1000) && modem.stream.available() < 0 ) yield();
        timeout = millis();
        while ((millis() - timeout < 1000)) {
            if (modem.stream.available() > 0) {
                res = modem.stream.readStringUntil('\n');
                telnet_println(res);
                timeout = millis();
            }
        }
    #endif
    yield();
}
