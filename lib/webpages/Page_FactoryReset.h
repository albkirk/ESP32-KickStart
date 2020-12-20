
//
//  HTML PAGE
//
const char PAGE_FactoryReset[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Factory Reset</strong>
<hr>
Return to Factory Defaults.<br>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:200px" >
<tr><td colspan="2" align="center"><a href="javascript:ResetState()" class="btn btn--m btn--blue">Reset</a></td></tr>
</table>


<script>

function ResetState()
{
	setValues("/admin/reset");
}

window.onload = function ()
{
  load("style.css","css", function() 
  {
    load("microajax.js","js", function() 
    {
    });
  });
}

function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}


</script>
)=====";


//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
//

void send_factory_reset_html()
{
		yield();
    MyWebServer.send ( 200, "text/html", PAGE_FactoryReset );
		Serial.println(__FUNCTION__);
}

void execute_factory_reset_html()
{
		Serial.println("Web Page RESET button pushed!!! ");
		storage_reset();
    MyWebServer.stop();
		ESPRestart();
}