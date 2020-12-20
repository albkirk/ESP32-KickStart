#include <TinyGPS++.h>


#define Cold_Start 30000                            // Typically ~27500 msec fix time from Cold start
#define Warm_Start 1000                             // Typically <1000  msec fix time from Warm start
#define Re_Acqu_Start 1000                          // Typically <1000  msec fix time from Re-Acquisition
#define AGPS_Start 10000                            // Typically <10000 msec fix time using Assisted GPS

#define GPS_WEB                                     // Flag to load GPS Page on WEB Service
#define GPS_cycle 1100                              // Setting 1000 milisecond by default -- it Depends on what's configured on GPS module!
#define GPS_Update 10                               // Value (in Seconds) to update GPS Data variables
static const uint32_t GPSBaud = 9600;
uint32_t GPS_Age, GPS_Sat;
double GPS_Lat, GPS_Lng, GPS_Alt, GPS_Course, GPS_Speed; // GPS data: Satellites, Latitude, Longitude, Altitude, Course and Speed
bool GPS_Valid = false;

// The TinyGPS++ object
TinyGPSPlus gps;

#ifdef ESP8266
    #include "SoftwareSerial.h"                     // for use with ESP8266
    SoftwareSerial ss();                            // The serial connection to the GPS device

#elif ESP32
    HardwareSerial serialGPS(2);                    // for use with ESP32 (1/2)
    #define ss serialGPS                            // The serial connection to the GPS device

#else
    #error “Unsupported board selected!”
#endif


// This custom version of delay() ensures that the gps object
// is being "fed".
static void gps_update(unsigned long ms = GPS_cycle)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);

  if(gps.location.isValid()) {
        GPS_Valid = true;
        GPS_Age = gps.satellites.age();
        GPS_Sat = gps.satellites.value();
        GPS_Lat = gps.location.lat();
        GPS_Lng = gps.location.lng();
        GPS_Alt = gps.altitude.meters();
        GPS_Course = gps.course.deg();
        GPS_Speed = gps.speed.kmph();
  }
  else {
        GPS_Valid = false;
        GPS_Age = 0;
        //GPS_Sat = 0;
        GPS_Lat = 0;
        GPS_Lng = 0;
        GPS_Alt = 0;
        GPS_Course = 0;
        GPS_Speed = 0;
  }
}

bool gps_detected() {
    unsigned long starttime = millis();
    while (!gps.satellites.isValid() && millis() - starttime < 2000)
    {
        gps_update();
    }
    if (!gps.satellites.isValid()) {
        telnet_println("No GPS data received: check wiring");
        return false;
    }
    else {
        telnet_println("GPS detected!");
        //telnet_println("GPS detected in " + String((millis() - starttime)) + " msec.");
        return true;
    }

}


void gps_start() {
    unsigned long starttime = millis();
    if (GPS_SW>=0) digitalWrite(GPS_SW, HIGH);                  // GPS Power on
    config.HW_Module = true;
    telnet_println("GPS power ON");
    if(gps_detected()) {
        while (!GPS_Valid && millis() - starttime < Cold_Start) // Worst scenario is cold_Starting
        {
            gps_update();
        }   
        if (GPS_Valid) telnet_println("GPS FIXed in " + String((millis() - starttime)/1000) + " sec.");
    }
}

void gps_stop() {
    if (GPS_SW>=0) digitalWrite(GPS_SW, LOW);                   // GPS Power off
    config.HW_Module = false;
    telnet_println("GPS power OFF");
}

void gps_setup() {
    telnet_print(F("TinyGPS++ library v. ")); telnet_println(TinyGPSPlus::libraryVersion());

    if (GPS_SW>=0) {
        pinMode(GPS_SW, OUTPUT);
        digitalWrite(GPS_SW, LOW);                              // Init GPS Power off
    }

    #ifdef ESP8266
        ss.begin(GPSBaud, SWSERIAL_8N1 , GPS_RX, GPS_TX);       // Use SWSERIAL_8N1 with SWserial. Use SERIAL_8N1 with  HWserial.
    #elif ESP32
        ss.begin(GPSBaud, SERIAL_8N1 , GPS_RX, GPS_TX);         // Use SWSERIAL_8N1 with SWserial. Use SERIAL_8N1 with  HWserial.
    #else
        #error “Unsupported board selected!”
    #endif

 /*  
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
  printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  printInt(gps.location.age(), gps.location.isValid(), 5);
  printDateTime(gps.date, gps.time);
  printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
  printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
  printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);

  unsigned long distanceKmToLondon = (unsigned long)TinyGPSPlus::distanceBetween(gps.location.lat(),gps.location.lng(),LONDON_LAT, LONDON_LON) / 1000;
  printInt(distanceKmToLondon, gps.location.isValid(), 9);

  double courseToLondon = TinyGPSPlus::courseTo(gps.location.lat(), gps.location.lng(), LONDON_LAT, LONDON_LON);
  printFloat(courseToLondon, gps.location.isValid(), 7, 2);

  const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);
  printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);
  printInt(gps.charsProcessed(), true, 6); printInt(gps.sentencesWithFix(), true, 10); printInt(gps.failedChecksum(), true, 9);
  telnet_println();
 */

}
