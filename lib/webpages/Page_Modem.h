//
//   HTML PAGE
//
const char PAGE_Modem[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Modem Configuration</strong>
<hr>
<form action="" method="post">
<hr>
<strong>Modem:</strong><br>
<table border="0"  cellspacing="0" cellpadding="3" >
<tr><td align="right">Model :</td><td><span id="modemInfo"></span></td></tr>
<tr><td align="right">IMEI :</td><td><span id="imei"></span></td></tr>
<tr><td align="right">State :</td><td><span id="state"></span></td></tr>
</table>

<hr>
<strong>SIM Card:</strong><br>
<table border="0"  cellspacing="0" cellpadding="3" >
<tr><td align="right">IMSI :</td><td><span id="imsi"></span></td></tr>
<tr><td align="right">CCID :</td><td><span id="ccid"></span></td></tr>
<tr><td align="right">Status :</td><td><span id="SIMStatus"></span></td></tr>
</table>

<hr>
<strong>Network:</strong><br>
<table border="0"  cellspacing="0" cellpadding="3" >
<tr>
    <td align="right">APN:</td>
    <td>
    <select  id="apn" name="apn">
    <option value="internet">internet</option>
    <option value="internetm2m">internetm2m</option>
    <option value="internet.vodafone.pt">internet.vodafone.pt</option>
    <option value="net2.vodafone.pt">net2.vodafone.pt</option>
    <option value="Internetfixa.vodafone.pt">Internetfixa.vodafone.pt</option>
    <option value="m2m.vodafone.pt">m2m.vodafone.pt</option>
    <option value="nbiot1.vodafone.pt">nbiot1.vodafone.pt</option>
    <option value="nbiot2.vodafone.pt">nbiot2.vodafone.pt</option>
    <option value="nbiot3.vodafone.pt">nbiot3.vodafone.pt</option>
    </select>
    </td>
</tr>

<tr>
    <td align="right">Username:</td>
    <td><input type="text" id="modemusername" name="modemusername" value=""></td>
</tr>
<tr>
    <td align="right">Password:</td>
    <td><input type="text" id="modempassword" name="modempassword" value=""></td>
</tr>

<tr><td align="right">Reg :</td><td><span id="regstatus"></span></td></tr>
<tr><td align="right">Oper :</td><td><span id="cop"></span></td></tr>
<tr><td align="right">RSSI :</td><td><span id="rssi"></span> dBm</td></tr>
<tr><td align="right">IP :</td><td><span id="modemip"></span></td></tr>
</table>

<hr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>

</form>
<script>



window.onload = function ()
{
    load("style.css","css", function()
    {
        load("microajax.js","js", function()
        {
            setValues("/admin/modemvalues");
        });
    });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====";


// Functions for this Page
void send_modem_html()
{
    if (MyWebServer.args() > 0 )  // Save Settings
    {
        String old_apn  = String(config.APN);
        String old_user = String(config.MODEM_User);
        String old_pass = String(config.MODEM_Password);
        String old_pin  = String(config.SIMCardPIN);

        for ( uint8_t i = 0; i < MyWebServer.args(); i++ ) {
            if (MyWebServer.argName(i) == "apn") strcpy(config.APN, urldecode(MyWebServer.arg(i)).c_str());
            if (MyWebServer.argName(i) == "modemusername") strcpy(config.MODEM_User, urldecode(MyWebServer.arg(i)).c_str());
            if (MyWebServer.argName(i) == "modempassword" && urldecode(MyWebServer.arg(i)) != "") strcpy(config.MODEM_Password, urldecode(MyWebServer.arg(i)).c_str());
        }
        if (old_apn != String(config.APN) || old_user != String(config.MODEM_User) || old_pass != String(config.MODEM_Password) || old_pin != String(config.SIMCardPIN)) {
            MODEM_Disconnect();
            if (MODEM_Connect()) {
                telnet_println("Connected with success using new data");
                storage_write();
                if (config.DEBUG) storage_print();
            }
            else {
                strcpy(config.APN, old_apn.c_str());
                strcpy(config.MODEM_User, old_user.c_str());
                strcpy(config.MODEM_Password, old_pass.c_str());
                strcpy(config.SIMCardPIN, old_pin.c_str());
                telnet_println("Failing connecting with new data. Restore previous config...");
            }
        }
    }

    if (!MyWebServer.authenticate(config.WEB_User, config.WEB_Password))  {
        MyWebServer.requestAuthentication();
    }
    else{
        MyWebServer.send ( 200, "text/html", PAGE_Modem );
    }
    MyWebServer.send ( 400, "text/html", "Authentication failed" );
    if (config.DEBUG) Serial.println(__FUNCTION__);

}


//
// FILL WITH INFOMATION
// 

void send_modem_values_html ()
{
    if (Modem_state >= 2) MODEM_Info();
    String values ="";

    values += "modemInfo|" + ModemInfo + "|div\n";
    values += "imei|" + ModemIMEI + "|div\n";
    values += "state|" + Modem_state_Name[Modem_state] + "|div\n";
    values += "imsi|" + SIMCardIMSI + "|div\n";
    values += "ccid|" + SIMCardCCID  + "|div\n";
    values += "SIMStatus|" + SimStatus_Name[modem.getSimStatus()] + "|div\n";
    values += "regstatus|" + RegStatus_string(modem.getRegistrationStatus()) + "|div\n";
    values += "apn|" +  String(config.APN) +  "|input\n";
    values += "modemusername|" +  String(config.MODEM_User) +  "|input\n";
    values += "modempassword|" +  String(config.MODEM_Password) +  "|input\n";      // KEEP IT COMMENTED TO NOT SHOW THE PASS!!!
    values += "cop|" + modem.getOperator() + "|div\n";
    values += "rssi|" + String(CSQ_to_RSSI(modem.getSignalQuality())) + "|div\n";
    values += "modemip|" + modem.localIP().toString() + "|div\n";
    MyWebServer.send ( 200, "text/plain", values);
    if (config.DEBUG) Serial.println(__FUNCTION__);
}
