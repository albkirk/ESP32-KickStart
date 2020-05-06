// Function to insert customized MQTT Configuration actions

void  mqtt_custom(String reqtopic, String reqparam, StaticJsonDocument<200> data) {
  if ( reqparam == "Info" && bool(data["value"])) {
    //status_report();
    if (BattPowered) mqtt_publish(mqtt_pathtele(), "BattLevel", String(getVoltage()));
    mqtt_publish(mqtt_pathtele(), "Switch", String(SWITCH));
    mqtt_publish(mqtt_pathtele(), "Timer", String(TIMER));
  }
  if ( reqparam == "Config" && bool(data["value"])) {
    mqtt_publish(mqtt_pathtele(), "OTA", String(config.OTA));
    mqtt_publish(mqtt_pathtele(), "TELNET", String(config.TELNET));
  }
  if ( reqparam == "Switch" && SWITCH_Last == bool(data["value"])) mqtt_publish(mqtt_pathtele(), "Switch", String(SWITCH));;
}
