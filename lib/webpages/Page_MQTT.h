//
//  HTML PAGE
//

const char PAGE_MQTTConfiguration[] PROGMEM =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>MQTT Configuration</strong>
<hr>
<form action="" method="post">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr>
    <td align="right">Server (IP or FQDN):</td>
    <td><input type="text" id="mqttserver" name="mqttserver" value=""></td>
</tr>
<tr>
    <td align="right">Port:</td>
    <td><input type="text" id="mqttport" name="mqttport" value=""></td>
</tr>
<tr><td align="right">Secure:</td><td><input type="checkbox" id="Secure" name="Secure"></td></tr>

<tr>
    <td align="right">Username:</td>
    <td><input type="text" id="mqttusername" name="mqttusername" value=""></td>
</tr>
<tr>
    <td align="right">Password:</td>
    <td><input type="text" id="mqttpassword" name="mqttpassword" value=""></td>
</tr>
<tr><td align="right">Status:</td><td><span id="mqtt_status"></span></td></tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<script>



window.onload = function ()
{
    load("style.css","css", function()
    {
        load("microajax.js","js", function()
        {
            setValues("/admin/mqttvalues");
        });
    });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====";


// Functions for this Page
void send_mqtt_html()
{

    if (MyWebServer.args() > 0 )  // Save Settings
    {
        config.MQTT_Secure = false;
        for ( uint8_t i = 0; i < MyWebServer.args(); i++ ) {
            if (MyWebServer.argName(i) == "mqttserver") strcpy(config.MQTT_Server, urldecode(MyWebServer.arg(i)).c_str());
            if (MyWebServer.argName(i) == "mqttport") config.MQTT_Port =  MyWebServer.arg(i).toInt();
            if (MyWebServer.argName(i) == "Secure") config.MQTT_Secure = true;
            if (MyWebServer.argName(i) == "mqttusername") strcpy(config.MQTT_User, urldecode(MyWebServer.arg(i)).c_str());
            if (MyWebServer.argName(i) == "mqttpassword" && urldecode(MyWebServer.arg(i)) != "") strcpy(config.MQTT_Password, urldecode(MyWebServer.arg(i)).c_str());
        }
        mqtt_disconnect();
        mqtt_set_client();
        mqtt_connect();
        if (MQTT_state == MQTT_CONNECTED) storage_write();
        if (config.DEBUG) storage_print();
    }

    if (!MyWebServer.authenticate(config.WEB_User, config.WEB_Password))  {
        MyWebServer.requestAuthentication();
    }
    else{
        MyWebServer.send ( 200, "text/html", PAGE_MQTTConfiguration );
    }
    MyWebServer.send ( 400, "text/html", "Authentication failed" );
    if (config.DEBUG) Serial.println(__FUNCTION__);

}


//
// FILL WITH INFOMATION
// 

void send_mqtt_values_html()
{
    String values ="";
    values += "mqttserver|" +  String(config.MQTT_Server) +  "|input\n";
    values += "mqttport|" +  String(config.MQTT_Port) +  "|input\n";
    values += "Secure|" + (String) (config.MQTT_Secure ? "checked" : "") + "|chk\n";
    values += "mqttusername|" +  String(config.MQTT_User) +  "|input\n";
    //values += "mqttpassword|" +  String(config.MQTT_Password) +  "|input\n";      // KEEP IT COMMENTED TO NOT SHOW THE WiFi KEY!!!
    values += "mqtt_status|" +  MQTT_state_string() +  "|div\n";
    MyWebServer.send ( 200, "text/plain", values);
    if (config.DEBUG) Serial.println(__FUNCTION__);
}
