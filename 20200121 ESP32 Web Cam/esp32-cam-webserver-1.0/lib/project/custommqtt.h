// Function to insert customized MQTT Configuration actions

void  mqtt_custom(String reqparam, String reqvalue, StaticJsonDocument<200> data) {
  if ( reqparam == "LED2") { config.LED = data["value"];storage_write(); }
}
