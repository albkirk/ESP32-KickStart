// **** Project code definition here ...
//#include <flash.h>
//#include <ambient.h>
#include <mygps.h>
#include <modem.h>
#include <HTTPClient.h>


//JSON Variables
char telemetry_jsonString[256];
DynamicJsonDocument telemetry_doc(256);


// **** Project code functions here ...
// **** Project code functions here ...
void print_gps_data () {
        if (GPS_Valid) {
            telnet_print("Age: " + String(GPS_Age) + "\t");
            telnet_print("Sat: " + String(GPS_Sat) + "\t");
            telnet_print("Lat: " + String(GPS_Lat, 4) + "\t");
            telnet_print("Lng: " + String(GPS_Lng, 4) + "\t");
            telnet_print("Alt: " + String(GPS_Alt, 1) + "\t");
            telnet_print("Course: " + String(GPS_Course, 1) + "\t");
            telnet_print("Speed: " + String(GPS_Speed, 1) + "\t");
            telnet_println("");
        }
        else telnet_println("GPS Not FIX");
}

bool modem_http_post() {
    const char APIHost[] = "casahassio.hopto.org";
    uint16_t APIPort     = 8123;

    if (modemWebClient.connect(APIHost, APIPort)) {
        telnet_println("Connected. Sending HTTP POST");

        String APIPath       = "/api/webhook/";
        String APIKey        = "0c3ef58716200912b665116e41dece2a2432d8123d6d55c4e5f6128b270a49e2";
        String uri           = APIPath + APIKey;

        String postData = "";
        postData += "latitude=" + String(GPS_Lat, 6);
        postData += "&longitude=" + String(GPS_Lng, 6);
        postData += "&device=Boris"; // + ChipID;
        postData += "&accuracy=" + String(millis()/60000);
        postData += "&battery=" + String(getBattLevel(),1);
        postData += "&speed=" + String(GPS_Speed, 1);
        postData += "&direction=" + String(GPS_Course, 1);
        postData += "&altitude=" + String(GPS_Alt, 1);
        postData += "&provider=T-CAll";
        postData += "&activity=Dog_walk";

        String HTTPPost = "";
        HTTPPost += "POST " + uri + " HTTP/1.1\r\n";
        HTTPPost += "Host: " + String(APIHost) +"\r\n";
        HTTPPost += "Content-Type: application/x-www-form-urlencoded\r\n";
        HTTPPost += "Content-Length: " + String(postData.length()) + "\r\n";
        HTTPPost += "\r\n";
        HTTPPost += postData;

        modemWebClient.println(HTTPPost);

        modemWebClient.stop();
        return true;
    } else {
        telnet_println("WEB connection / HTTP POST failed");
        return false;
    }
}


void send_Telemetry() {
/*
    char fbat[3];        // long enough to hold complete floating string
    // Purge old JSON data and Load new values
    telemetry_doc.clear();
    telemetry_doc["Interval"] = config.SLEEPTime;
    telemetry_doc["Timestamp"] = curUnixTime();
    telemetry_doc["BatLevel"] = String(dtostrf(getBattLevel(),3,0,fbat)).toFloat();
    telemetry_doc["RSSI"] = getRSSI();
    if (config.HW_Module){
        gps_update();
        if (GPS_Valid){
            telemetry_doc["Sat"] = String(GPS_Sat).toInt();
            telemetry_doc["Lat"] = String(GPS_Lat, 6).toFloat();
            telemetry_doc["Lng"] = String(GPS_Lng, 6).toFloat();
            telemetry_doc["Speed"] = String(GPS_Speed, 1).toFloat();
        }
    }

    serializeJson(telemetry_doc, telemetry_jsonString);             //Serialize JSON data to string
    //telnet_println("Telemetry: " + String(telemetry_jsonString));
    mqtt_publish(mqtt_pathtele, "Telemetry", String(telemetry_jsonString));
*/

    //gps_update();
    if (GPS_Valid && Celular_Connected) modem_http_post();
    else telnet_println("NO Valid conditions to send data");
}

void project_hw() {
 // Output GPIOs


 // Input GPIOs

    //flash_setup();                                // Required for telemetry storage in Flash
}


void project_setup() {
  // Start Ambient devices
      //ambient_setup();
      //TIMER = (ulong)config.SLEEPTime;            // TIMER value (Recommended 15 minutes) to get Ambient data.
      //ambient_data();

  // GPS Setup
    gps_setup();

  // Modem Setup
    modem_setup();

  // Starting Service ... if it is the right scenario 
    if (WIFI_state != WL_CONNECTED) {
        gps_start();
        if (config.DEEPSLEEP && !config.WEB) gps_stop();
        print_gps_data();
        if (GPS_Valid) modem_start();
        if (Celular_Connected) send_Telemetry();
        modem_stop();
        // config.SLEEPTime = 2;                // suggested to use 2 minutes for optimum\ Map update vs Battery live
    }
    else {
        SLEEPTime = 0;
        telnet_println("connected to WiFi (i.e. @ home!!). NO need to enable GPS or Modem");
    }
    /*if ( Celular_Connected && (MQTT_state != MQTT_CONNECTED) ) {
        MODEM_mqtt_client();
        mqtt_connect();
    }*/
}

void project_loop() {
  // Ambient handing
      //if (TIMER >0) if ((millis() - 3500) % (TIMER * 60000) < 5) ambient_data();      // TIMER bigger than zero on div or dog bites!!

  // Modem handling
    modem_loop();

  // Send Telemetry data every SLEEPTime minutes 
    if (config.SLEEPTime >0) if ((millis() + 3500) % (config.SLEEPTime * 60000) < 5) {
        if (config.DEEPSLEEP && !config.WEB) {
            gps_start();
            gps_stop();
        }
        else gps_update();
        print_gps_data();
        if (GPS_Valid) modem_start();
        if (Celular_Connected) send_Telemetry();
        modem_stop();
    }
}
