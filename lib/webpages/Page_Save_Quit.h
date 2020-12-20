
//
//  HTML PAGE
//
const char PAGE_Save_Quit[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Save & Quit</strong>
<hr>
Save and quit to start in normal operation.<br>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:200px" >
<tr><td colspan="2" align="center"><a href="javascript:SaveQuit()" class="btn btn--m btn--blue">Go!</a></td></tr>
</table>


<script>

function SaveQuit()
{
    setValues("/admin/savequit");
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

const char PAGE_SaveAndQuit[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="5; URL=wireless.html">
The Web Server is no longer enabled. Please, close this web page.
)=====";


//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
//

void send_save_quit_html()
{
        yield();
    MyWebServer.send ( 200, "text/html", PAGE_Save_Quit );
        telnet_println(__FUNCTION__);
}

void execute_save_quit_html()
{
        telnet_println("Web Page Save & Quit button pushed!!! ");
         MyWebServer.send ( 200, "text/html", PAGE_SaveAndQuit );

    MyWebServer.stop();
    config.TELNET = false;
    config.OTA = false;
    config.WEB = false;
    config.APMode = false;
    config.LED = false;
    storage_write();
        ESPRestart();
}