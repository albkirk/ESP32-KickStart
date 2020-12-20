
const char PAGE_NTPConfiguration[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>NTP Configuration</strong>
<hr>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr><td align="right">NTP Server:</td><td><input type="text" id="ntpserver" name="ntpserver" maxlength="172" value=""></td></tr>
<tr><td align="right">Update:</td><td><input type="text" id="update" name="update" size="3"maxlength="6" value=""> minutes</td></tr>
<tr><td>Timezone:</td><td>
<select  id="tz" name="tz">
  <option value="-120">(GMT-12:00)</option>
  <option value="-110">(GMT-11:00)</option>
  <option value="-100">(GMT-10:00)</option>
  <option value="-90">(GMT-09:00)</option>
  <option value="-80">(GMT-08:00)</option>
  <option value="-70">(GMT-07:00)</option>
  <option value="-60">(GMT-06:00)</option>
  <option value="-50">(GMT-05:00)</option>
  <option value="-40">(GMT-04:00)</option>
  <option value="-35">(GMT-03:30)</option>
  <option value="-30">(GMT-03:00)</option>
  <option value="-20">(GMT-02:00)</option>
  <option value="-10">(GMT-01:00)</option>
  <option value="0">(GMT+00:00)</option>
  <option value="10">(GMT+01:00)</option>
  <option value="20">(GMT+02:00)</option>
  <option value="30">(GMT+03:00)</option>
  <option value="35">(GMT+03:30)</option>
  <option value="40">(GMT+04:00)</option>
  <option value="45">(GMT+04:30)</option>
  <option value="50">(GMT+05:00)</option>
  <option value="55">(GMT+05:30)</option>
  <option value="57">(GMT+05:45)</option>
  <option value="60">(GMT+06:00)</option>
  <option value="65">(GMT+06:30)</option>
  <option value="70">(GMT+07:00)</option>
  <option value="80">(GMT+08:00)</option>
  <option value="90">(GMT+09:00)</option>
  <option value="95">(GMT+09:30)</option>
  <option value="100">(GMT+10:00)</option>
  <option value="110">(GMT+11:00)</option>
  <option value="120">(GMT+12:00)</option>
  <option value="120">(GMT+12:00)</option>
  <option value="130">(GMT+13:00)</option>
</select>
</td></tr>
<tr><td align="right">Daylight :</td><td><input type="checkbox" id="dst" name="dst"></td></tr>
<tr><td align="right">NTP Sync:</td><td><input type="checkbox" checked disabled id="sync" name="sync"></td></tr>
<tr><td align="right">Date/Time:</td><td><span id="x_ntp"></span></td></tr>

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
        setValues("/admin/ntpvalues");
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====";


void send_NTP_configuration_html()
{
  if (MyWebServer.args() > 0 )  // Save Settings
  {
    config.isDayLightSaving = false;
    String temp = "";
    for ( uint8_t i = 0; i < MyWebServer.args(); i++ ) {
      if (MyWebServer.argName(i) == "ntpserver") strcpy(config.NTPServerName, urldecode( MyWebServer.arg(i)).c_str());
      if (MyWebServer.argName(i) == "update") config.Update_Time_Via_NTP_Every =  MyWebServer.arg(i).toInt(); 
      if (MyWebServer.argName(i) == "tz") config.TimeZone =  MyWebServer.arg(i).toInt(); 
      if (MyWebServer.argName(i) == "dst") config.isDayLightSaving = true; 
    }
    storage_write();
    if (config.DEBUG) storage_print();
    firstStart = true;
  }
  MyWebServer.send ( 200, "text/html", PAGE_NTPConfiguration ); 
  Serial.println(__FUNCTION__); 
  
}



void send_NTP_configuration_values_html()
{

  getNTPtime();
  curDateTime();
  String values ="";
  values += "ntpserver|" + (String) config.NTPServerName + "|input\n";
  values += "update|" + (String) config.Update_Time_Via_NTP_Every + "|input\n";
  values += "tz|" + (String) config.TimeZone + "|input\n";
  values += "dst|" + (String) (config.isDayLightSaving ? "checked" : "") + "|chk\n";
  values += "sync|" + (String) (NTP_Sync ? "checked" : "") + "|chk\n";
  values += "x_ntp|" + (String) WeekDays[DateTime.wday] + ", " + (String) DateTime.year + "/" + (String) DateTime.month + "/" + (String) DateTime.day + "   " + (String) DateTime.hour + ":" + (String) DateTime.minute + ":" + (String)  DateTime.second + "|div\n";
  MyWebServer.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__); 

}
