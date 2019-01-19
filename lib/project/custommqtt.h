// Function to insert customized MQTT Configuration actions

void  mqtt_custom(String reqparam, String reqvalue, JsonObject& data) {
  if ( reqparam == "LED2") { config.LED = data["value"];storage_write(); }
}
