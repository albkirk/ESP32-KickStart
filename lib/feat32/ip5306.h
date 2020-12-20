#include <Wire.h>
#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00
#define IP5306_REG_READ8     0x78

bool setPowerBoostKeepOn(bool en=true)
{
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(IP5306_REG_SYS_CTL0);
  if (en) {
    Wire.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
  } else {
    Wire.write(0x35); // 0x37 is default reg value
  }
  return Wire.endTransmission() == 0;
}

int8_t GetIP5306BattLevel()
{
  Wire.beginTransmission(0x75);
  Wire.write(IP5306_REG_READ8);
  if (Wire.endTransmission(false) == 0
   && Wire.requestFrom(0x75, 1)) {
    switch (Wire.read() & 0xF0) {
    case 0xE0: return 25;
    case 0xC0: return 50;
    case 0x80: return 75;
    case 0x00: return 100;
    default: return 0;
    }
  }
  return -1;
}

bool ip5306_setup() {
    Wire.begin(SDAPIN, SCKPIN);
    return setPowerBoostKeepOn();
}
