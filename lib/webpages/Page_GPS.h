//
//  HTML PAGE
//

const char PAGE_GPSSettings[] PROGMEM =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>GPS Configuration</strong>
<hr>
<form action="" method="post">
<hr>
<strong>GPS</strong><br>
<table border="0"  cellspacing="0" cellpadding="3" >
<tr><td align="right">Sat:</td><td><span id="x_sat"></span></td></tr>
<tr><td align="right">Lat:</td><td><span id="x_lat"></span></td></tr>
<tr><td align="right">Lng:</td><td><span id="x_lng"></span></td></tr>
<tr><td align="right">Alt:</td><td><span id="x_alt"></span></td></tr>
<tr><td align="right">Coarse:</td><td><span id="x_crs"></span></td></tr>
<tr><td align="right">Speed:</td><td><span id="x_spd"></span></td></tr>
<tr><td align="right">Age:</td><td><span id="x_age"></span></td></tr>
</table>
<hr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Refresh"></td></tr>
</form>
<script>



window.onload = function ()
{
    load("style.css","css", function()
    {
        load("microajax.js","js", function()
        {
            setValues("/admin/gpsvalues");
        });
    });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====";


// Functions for this Page
void send_gps_html()
{
    if (MyWebServer.args() > 0 )  // Save Settings
    {
        //config.HW_Module = false;
        for ( uint8_t i = 0; i < MyWebServer.args(); i++ ) {
            yield();
        }
        //storage_write();
        //if (config.DEBUG) storage_print();
    }
    if (!MyWebServer.authenticate(config.WEB_User, config.WEB_Password))  {
        MyWebServer.requestAuthentication();
    }
    else{
        MyWebServer.send ( 200, "text/html", PAGE_GPSSettings );
    }
    MyWebServer.send ( 400, "text/html", "Authentication failed" );
    if (config.DEBUG) Serial.println(__FUNCTION__);


}

void send_gps_values_html()
{
    gps_update();
    String values ="";
    values += "x_sat|" + String(GPS_Sat) +  "|div\n";
    values += "x_lat|" + String(GPS_Lat, 6) +  "|div\n";
    values += "x_lng|" + String(GPS_Lng, 6) +  "|div\n";
    values += "x_alt|" + String(GPS_Alt, 1) +  "|div\n";
    values += "x_crs|" + String(GPS_Course, 1) +  "|div\n";
    values += "x_spd|" + String(GPS_Speed, 1) +  "|div\n";
    values += "x_age|" + String(GPS_Age) + "|div\n";
    MyWebServer.send ( 200, "text/plain", values);
    if (config.DEBUG) Serial.println(__FUNCTION__);
}
