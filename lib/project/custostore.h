// Function to insert customized STORAGE actions

void custo_JSON_TO_STRUCT() {
  config.Volume                 = root["Volume"];
}

void custo_STRUCT_TO_JSON() {
  root["Volume"]                = config.Volume;
}
