#include <WiFiUdp.h>

#define LEAP_YEAR(Y) ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )

struct  strDateTime
{
  byte hour;
  byte minute;
  byte second;
  int year;
  byte month;
  byte day;
  byte wday;
} ;

strDateTime DateTime;                         // Global DateTime structure
strDateTime LastDateTime = {25, 60, 60, 1, 13, 32, 8};
static const uint8_t monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const String WeekDays[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE];
volatile unsigned long UTCTimeStamp = 0;      // GLOBAL TIME var ( Will be retrieved via NTP protocol)
volatile unsigned long UnixTimeStamp = 0;     // GLOBAL TIME var ( Will be devivated from UTCTimeStamp for local time zone)
unsigned long RefMillis = 0;                  // Millis val for reference
boolean NTP_Sync = false;                     // NTP is synched?
unsigned int NTP_Retry = 300;                 // Timer to retry the NTP connection
long NTP_LastTime = 0;                        // Last NTP connection attempt time stamp
int NTP_errors = 0;                           // NTP errors Counter
volatile unsigned long cur_unixtime;          // Auxiliary var
volatile unsigned long ntpNOW = 0;            // Auxiliary var for millis


WiFiUDP UDPNTPClient;                         // NTP Client


strDateTime ConvertTimeStamp( unsigned long _tempTimeStamp) {
  strDateTime _tempDateTime;
  uint8_t year;
  uint8_t month, monthLength;
  uint32_t time;
  unsigned long days;

  time = (uint32_t)_tempTimeStamp;
  _tempDateTime.second = time % 60;
  time /= 60; // now it is minutes
  _tempDateTime.minute = time % 60;
  time /= 60; // now it is hours
  _tempDateTime.hour = time % 24;
  time /= 24; // now it is days
  _tempDateTime.wday = ((time + 4) % 7) + 1;  // Sunday is day 1

  year = 0;
  days = 0;
  while ((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
    year++;
  }
  _tempDateTime.year = year;                  // year is offset from 1970

  days -= LEAP_YEAR(year) ? 366 : 365;
  time  -= days;                              // now it is days in this year, starting at 0

  days = 0;
  month = 0;
  monthLength = 0;
  for (month = 0; month < 12; month++) {
    if (month == 1) { // february
      if (LEAP_YEAR(year)) {
        monthLength = 29;
      } else {
        monthLength = 28;
      }
    } else {
      monthLength = monthDays[month];
    }

    if (time >= monthLength) {
      time -= monthLength;
    } else {
      break;
    }
  }
  _tempDateTime.month = month + 1;            // jan is month 1
  _tempDateTime.day = time + 1;               // day of month
  _tempDateTime.year += 1970;

  return _tempDateTime;
}


//
// Summertime calculates the daylight saving time for middle Europe. Input: Unixtime in UTC
//
boolean summerTime(unsigned long _timeStamp ) {
  strDateTime  _tempDateTime = ConvertTimeStamp(_timeStamp);
  // printTime("Innerhalb ", _tempDateTime);

  if (_tempDateTime.month < 3 || _tempDateTime.month > 10) return false; // NOT Summertime in Jan, Feb, Nov, Dez
  if (_tempDateTime.month > 3 && _tempDateTime.month < 10) return true; // Summertime in Apr, Mai, Jun, Jul, Aug, Sep
  if (_tempDateTime.month == 3 && ((_tempDateTime.hour + 24 * _tempDateTime.day) >= (3 +  24 * (31 - (5 * _tempDateTime.year / 4 + 4) % 7))))
    return true;
  else if (_tempDateTime.month == 10 && ((_tempDateTime.hour + 24 * _tempDateTime.day) < (3 +  24 * (31 - (5 * _tempDateTime.year / 4 + 1) % 7))))
    return true;
  else
    return false;
}

unsigned long adjustTimeZone(unsigned long _timeStamp, int _timeZone, bool _isDayLightSavingSaving) {
  //strDateTime _tempDateTime;
  _timeStamp += _timeZone * 10 * 360;             // adjust timezone
  // printTime("Innerhalb adjustTimeZone ", ConvertTimeStamp(_timeStamp));
  if (_isDayLightSavingSaving && summerTime(_timeStamp)) _timeStamp += 3600; // adding summerTime hour
  return _timeStamp;
}


void getNTPtime()
{
    UTCTimeStamp = 0;                             // Resetting value to 0

    if (WiFi.status() == WL_CONNECTED) {
          UDPNTPClient.begin(2390);               // Port for NTP receive
          IPAddress timeServerIP;
          WiFi.hostByName(config.NTPServerName.c_str(), timeServerIP);

          //Serial.println("sending NTP packet...");
          memset(packetBuffer, 0, NTP_PACKET_SIZE);
          packetBuffer[0] = 0b11100011;             // LI, Version, Mode
          packetBuffer[1] = 0;                      // Stratum, or type of clock
          packetBuffer[2] = 6;                      // Polling Interval
          packetBuffer[3] = 0xEC;                   // Peer Clock Precision
          packetBuffer[12]  = 49;
          packetBuffer[13]  = 0x4E;
          packetBuffer[14]  = 49;
          packetBuffer[15]  = 52;
          UDPNTPClient.beginPacket(timeServerIP, 123);
          UDPNTPClient.write(packetBuffer, NTP_PACKET_SIZE);
          UDPNTPClient.endPacket();

          delay(100);

          int cb = UDPNTPClient.parsePacket();
          if (cb == 0) {
              Serial.println("No NTP packet yet");
          }
          else {
              Serial.print("NTP packet received, length=");
              Serial.println(cb);
              UDPNTPClient.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
              RefMillis = millis();           // Exact moment that NTP data was retrived
              unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
              unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
              unsigned long secsSince1900 = highWord << 16 | lowWord;
              const unsigned long seventyYears = 2208988800UL;
              UTCTimeStamp = secsSince1900 - seventyYears;      // store "Coordinated Universal Time" (UTC) time stamp
          }

    }
    else {
          Serial.println("Internet yet not connected");
          delay(500);
    }
    yield();
    if (UTCTimeStamp > 0) {
        UnixTimeStamp = adjustTimeZone(UTCTimeStamp, config.TimeZone, config.isDayLightSaving);
        NTP_Sync = true;
    }
    else NTP_Sync = false;
}

unsigned long curUnixTime() {
    ntpNOW = millis();
    if (ntpNOW < RefMillis) getNTPtime();
    cur_unixtime = UnixTimeStamp + (ntpNOW - RefMillis)/1000;
    //telnet_println("Current UNIX time: " + String(cur_unixtime));
    return cur_unixtime;
}


void curDateTime() {
    cur_unixtime = curUnixTime();
    DateTime = ConvertTimeStamp(cur_unixtime);
    //telnet_println("Current Local Date: " + String(DateTime.year) + "/" + String(DateTime.month) + "/" + String(DateTime.day));
    //telnet_println("Current Local Time: " + String(DateTime.hour) + ":" + String(DateTime.minute) + ":" + String(DateTime.second));
}

byte DateTime_hour(void) {return DateTime.hour;}
byte DateTime_minute(void) {return DateTime.minute;}
byte DateTime_second(void) {return DateTime.second;}

void GoingToSleep(byte Time_minutes = 0) {
  //rtcData.lastUTCTime = curUnixTime();
  //RTC_write();
  ESP.deepSleep( Time_minutes * 60 * 1000000);   // time in minutes converted to microseconds
}


void ntp_setup () {
    getNTPtime();
    if (NTP_Sync) {
        curDateTime();
        telnet_println("Current Local Date: " + String(DateTime.year) + "/" + String(DateTime.month) + "/" + String(DateTime.day));
        telnet_println("Current Local Time: " + String(DateTime.hour) + ":" + String(DateTime.minute) + ":" + String(DateTime.second));
    }
}


void ntp_loop () {
  if (!NTP_Sync) {
      if ( millis() - NTP_LastTime > (NTP_Retry * 1000)) {
          NTP_errors ++;
          Serial.println( "in loop function NTP NOT sync! #: " + String(NTP_errors));
          NTP_LastTime = millis();
          getNTPtime();
          if (NTP_Sync) curDateTime();
      }
  }
  yield();
}
