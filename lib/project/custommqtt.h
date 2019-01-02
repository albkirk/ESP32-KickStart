// Function to insert customized MQTT Configuration actions

void  mqtt_custom(String reqparam, String reqvalue, JsonObject& data) {
  if ( reqparam == "Volume") {config.Volume = byte((long(data["value"]) / 1 * 1)); storage_write(); }
}
