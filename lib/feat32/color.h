/*      H O W  T O   U S E   t H I S   L I B R A R Y ?
Add these 3 lines to your main.cpp file:
#define NEOPixelsPIN  19    // Which pin on the ESP32 is connected to the NeoPixels?
#define NEOPixelsNUM  8     // How many NeoPixels LEDs are attached to the ESP32?
#include <color.h>

On SETUP function add this line:
color_setup();

On LOOP function add this line:
color_loop();

change color by changing the value of config.color
Change Light brightness by changing the value of: GAIN


*/

//#include <FastLED.h>
//#include <NeoPixelBus.h>
#include <Adafruit_NeoPixel.h>


// CONSTANTs
    #define PWM_RANGE 255                     // Duty cycle value 255 equalto FF
    #define PWM_FREQ 250                      // PWM Frequency

    char BLACK[10] =        "#00000000";      // RGB code for BLACK or LED Strip OFF
    char WHITE[10] =        "#FFFFFFFF";      // RGB code for WHITE
    char RED[10] =          "#FF0000FF";      // RGB code for RED
    char GREEN[10] =        "#00FF00FF";      // RGB code for GREEN
    char BLUE[10] =         "#0000FFFF";      // RGB code for BLUE
    char ORANGE[10] =       "#FFA500FF";      // RGB code for ORANGE
    char LIGHT_GREEN[10] =  "#90EE90FF";      // RGB code for LIGHT GREEN
    char DEEP_BLUE[10] =    "#000080FF";      // RGB code for NAVY
    char MILK_WHITE[10] =   "#FFF0F0FF";      // RGB code for SNOW
    char DEEP_YELLOW[10] =  "#FFD700FF";      // RGB code for GOLD
    char CYAN[10] =         "#00FFFFFF";      // RGB code for CYAN
    char BLUE_PURPLE[10] =  "#9370DBFF";      // RGB code for MEDIUM PURPLE
    char PINK_WHITE[10] =   "#FFC0CBFF";      // RGB code for PINK
    char YELLOW[10] =       "#FFFF00FF";      // RGB code for YELLOW
    char LIGHT_BLUE[10] =   "#ADD8E6FF";      // RGB code for LIGHT BLUE
    char PURPLE[10] =       "#800080FF";      // RGB code for PURPLE
    char GREEN_WHITE[10] =  "#E0FFE0FF";      // RGB code for HONEYDEW
    char LIGHT_YELLOW[10] = "#FFFFD0FF";      // RGB code for LIGHT YELLOW
    char SKY_BLUE[10] =     "#87CEEBFF";      // RGB code for SKY BLUE
    char BROWN[10] =        "#A52A2AFF";      // RGB code for BROWN
    char BLUE_WHITE[10] =   "#E0E0FFFF";      // RGB code for ALICE BLUE

    // Rainbow colors: red, orange, yellow, green, blue, indigo, and violet
    char *rainbow[] = {RED, ORANGE, YELLOW, GREEN, BLUE, CYAN, PURPLE};

//
// VARIABLES
//
    char Color[10]     = "#00000000"; // RGB Color code. syntax: '#' + RED + GREEN + BLUE + Transparency
    char LastColor[10] = "#00000000"; // each param use 2 CHARs and range from 0 to FF (HEX format of 0-255).
                                      // Transparency is ignored. It's kept only for compatibility purposes.
    byte RGB[3];                      // RRB trio byte Values to control the OUTPut PWM PINs
    byte neoID = 0;                    // NEO Pixel ID [0 - n] to change color
    byte zeros[3] = {0, 0, 0};        // trio of byte Value 0 to represnt Black color
    char Xval[2];                     // Buffer memory to convert byte into double HEX char
    char *Xarr;
    long RANGE[] = {15, 31, 47, 63, 79, 95, 111, 127, 143, 159, 175, 191, 207, 223, 239, 255};
    long GAIN = 100;                  // Color gain 0 to 100%
// EFX
    const String EFXName[] = {"NoEFX", "Auto", "Flash", "Fade3", "Fade7", "Jump3", "Jump7", "Raibow", "Scan"};
    byte EFX = 0;                     // The EFX to be played
    byte EFX_RGB[3] = {0, 0, 0};      // Effects RGB trio byte values used during Effects
    int  EFX_idx=0;                   // RGB array index
    long EFX_Delta=-2;                // RGB delta value change (-2 or +2)
    long EFX_GAIN = 100;              // Effect Color gain 0 to 100%
    uint EFX_Interval=1500;           // Effect interval timer in milliseconds
    uint Last_EFX=0;                  // last time Effect status was changed
    uint Last_Flash=0;                // last time Flash status was changed
    bool Bool_flash=false;            // Boolean to toggle flash status
    byte RainBow_idx = 0;               // Raibow color index

    // When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
    // Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
    // example for more information on possible values.
    Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NEOPixelsNUM, NEOPixelsPIN, NEO_GRB + NEO_KHZ800);

    //NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> pixels(NEOPixelsNUM, NEOPixelsPIN);

/*
// Create Global COLOR related variables
    char Color[10]     = "#00000000"; // RGB Color code. syntax: '#' + RED + GREEN + BLUE + Transparency
    char LastColor[10] = "#00000000"; // each param use 2 CHARs and range from 0 to FF (HEX format of 0-255).
                                      // Transparency is ignored. It's kept only for compatibility purposes.
*/

//
//  AUXILIAR functions
//
byte CharHEX_to_byte(char X) {
  byte num;
  if (byte(X) >= 48 && byte(X) <= 57) num = byte(X)- 48;   // 48 ='0' and 57 = '9'
  else if (byte(X) >= 65 && byte(X) <= 70) num = byte(X) - 65 + 10;   // 65 = 'A'  and 70 ='F'
  else if (byte(X) >= 97 && byte(X) <= 102) num = byte(X) - 97 + 10;   // 97 = 'a'  and 102 ='f'
  else num = 0;
  //Serial.print(" ->" + String(num));
  return num;
}


char *byte_to_CharHEX(byte Bval, char arr[2]) {
  if (Bval/16 >= 0 && Bval/16 <= 9) arr[0] =char(48 + Bval/16);   // 48 ='0' and 57 = '9'
  else if (Bval/16 >= 10 && Bval/16 <= 15) arr[0] = char(65 + Bval/16 - 10);   // 65 = 'A'  and 70 ='F'
  if (Bval%16 >= 0 && Bval%16 <= 9) arr[1] = char(48 + Bval%16);   // 48 ='0' and 57 = '9'
  else if (Bval%16 >= 10 && Bval%16 <= 15) arr[1] = char(65 + Bval%16 - 10);   // 65 = 'A'  and 70 ='F'
  return arr;

  // then you want to use it.
//Xarr = byte_to_CharHEX(255, Xval);
}

bool barraycpy (byte *arrout, byte *arrin) {
  if (sizeof(arrin) > sizeof(arrout)) return false;
  for (size_t i = 0; i < 3; i++) {
    arrout[i] = arrin[i];
  }
  return true;
}


byte color_gain (byte Cval, int Gval) {
   return (byte) (Cval * Gval/100.0);
}

void RGBcolor_paint (byte PAINT[3], int PGain = GAIN) {
/*  // analogwrite() NOT supported on ESP32?!
  analogWrite(PIN_RED,   color_gain(PAINT[0], PGain));      // OUTPUT RED level
  analogWrite(PIN_GREEN, color_gain(PAINT[1], PGain));      // OUTPUT GREEN level
  analogWrite(PIN_BLUE,  color_gain(PAINT[2], PGain));      // OUTPUT BLUE level
*/
}

void NEOcolor_paint (byte PAINT[3], int PGain = GAIN, byte nID = 0) {
  pixels.setPixelColor(nID, pixels.Color(color_gain(PAINT[0], PGain), color_gain(PAINT[1], PGain), color_gain(PAINT[2], PGain))); // Moderately bright green color.
  //pixels.SetPixelColor(nID, RgbColor(color_gain(PAINT[0], PGain), color_gain(PAINT[1], PGain), color_gain(PAINT[2], PGain))); // Moderately bright green color.
  pixels.show(); // This sends the updated pixel color to the hardware.
}

bool RGBcolor_set (char color_code[10]) {
    if (color_code[0] == '#') {
        RGB[0] = CharHEX_to_byte(color_code[1]) * 16 + CharHEX_to_byte(color_code[2]);
        //telnet_println(String(byte_to_CharHEX(RGB[0], Xval)));
        RGB[1] = CharHEX_to_byte(color_code[3]) * 16 + CharHEX_to_byte(color_code[4]);
        RGB[2] = CharHEX_to_byte(color_code[5]) * 16 + CharHEX_to_byte(color_code[6]);
        //telnet_println(" R=" + String(RGB[0]) + " G=" + String(RGB[1]) + " B=" + String(RGB[2]));
        //Serial.println(String(Color) + " : " + String(color_code) + " : " + String(LastColor));
        RGBcolor_paint (RGB, GAIN);
        strcpy(LastColor,color_code);
        barraycpy(EFX_RGB, RGB);
        EFX_GAIN = GAIN;
        return true;
    }
    else return false;
}

bool NEOcolor_set (char color_code[10], byte nID = 0) {
    if (color_code[0] == '#') {
        RGB[0] = CharHEX_to_byte(color_code[1]) * 16 + CharHEX_to_byte(color_code[2]);
        //telnet_println(String(byte_to_CharHEX(RGB[0], Xval)));
        RGB[1] = CharHEX_to_byte(color_code[3]) * 16 + CharHEX_to_byte(color_code[4]);
        RGB[2] = CharHEX_to_byte(color_code[5]) * 16 + CharHEX_to_byte(color_code[6]);
        //telnet_println(" R=" + String(RGB[0]) + " G=" + String(RGB[1]) + " B=" + String(RGB[2]));
        //Serial.println(String(Color) + " : " + String(color_code) + " : " + String(LastColor));
        NEOcolor_paint (RGB, GAIN, nID);
        //strcpy(LastColor,color_code);
        //barraycpy(EFX_RGB, RGB);
        //EFX_GAIN = GAIN;
        return true;
    }
    else return false;
}


// Color Effect functions
void color_Auto() {
    if ( millis() - Last_EFX > EFX_Interval/10) {
      int idx = random(0,3);
      int deviate = random(-5,6);
      if (EFX_RGB[idx] + deviate >=0 && EFX_RGB[idx] + deviate <=255) {
          EFX_RGB[idx] += deviate ;
          RGBcolor_paint(EFX_RGB, EFX_GAIN);
      }
      Last_EFX = millis();
    }
}

void color_Flash() {
    if ( millis() - Last_Flash > EFX_Interval/4) {
        if (Bool_flash) {
            EFX_GAIN = 0;
            RGBcolor_paint(EFX_RGB, EFX_GAIN);
        }
        else {
            EFX_GAIN = GAIN;
            RGBcolor_paint(EFX_RGB, EFX_GAIN);
        }
        Bool_flash = !Bool_flash;
        Last_Flash = millis();
    }
}

void color_Fade3() {
    if ( millis() - Last_EFX > EFX_Interval/20) {
      if (EFX_RGB[EFX_idx] + EFX_Delta >=0 && EFX_RGB[EFX_idx] + EFX_Delta <=RGB[EFX_idx]) {
          EFX_RGB[EFX_idx] += EFX_Delta;
          RGBcolor_paint(EFX_RGB, EFX_GAIN);
      }
      else {
        if (EFX_Delta <0) EFX_idx = (EFX_idx +1) % 3;
        EFX_Delta = - EFX_Delta;
      }
      Last_EFX = millis();
    }
}

void color_Fade7() {
    if ( millis() - Last_EFX > EFX_Interval/20) {
      if (EFX_RGB[EFX_idx] + EFX_Delta >=0 && EFX_RGB[EFX_idx] + EFX_Delta <=RGB[EFX_idx]) {
          EFX_RGB[EFX_idx] += EFX_Delta;
          RGBcolor_paint(EFX_RGB, EFX_GAIN);
      }
      else {
        EFX_idx = (EFX_idx +1) % 3;
        EFX_Delta = - EFX_Delta;
      }
      Last_EFX = millis();
    }
}

void color_rainbow() {
    if ( millis() - Last_EFX > EFX_Interval/20) {
      NEOcolor_set(rainbow[RainBow_idx], neoID);
      NEOcolor_set(rainbow[RainBow_idx], neoID+1);
      //Serial.println("Color" + String(rainbow[RainBow_idx]) + "  NEO: " + neoID);
      RainBow_idx = (RainBow_idx + 1) % (sizeof(rainbow)/sizeof(*rainbow));
      neoID  = (neoID + 2)% NEOPixelsNUM;
      Last_EFX = millis();
    }
}

void color_scan() {
    if ( millis() - Last_EFX > EFX_Interval/20) {
      NEOcolor_set(BLACK, neoID);
      RainBow_idx = (RainBow_idx + 1) % (sizeof(rainbow)/sizeof(*rainbow));
      neoID  = (neoID + 1) % NEOPixelsNUM;
      NEOcolor_set( rainbow[RainBow_idx], neoID);
      Last_EFX = millis();
    }
}

//
//  MAIN functions
//
void color_setup() {
    // ***********  Color SETUP
    /*
    pinMode(PIN_RED,   OUTPUT);
    pinMode(PIN_GREEN, OUTPUT);
    pinMode(PIN_BLUE,  OUTPUT);
    analogWriteRange(PWM_RANGE);      // Duty cycle Range of values [o-PWMRANGE] equal to FF
    analogWriteFreq(PWM_FREQ);        // PWM Frequency 250Hz
    */
    // Set NeoPixel configuration
    //pixels.SetBrightness(BRIGHTNESS);

    // Start NeoPixel library with all LEDs off
    pixels.begin();                     // This initializes the NeoPixel library.
    //RGBcolor_set(BLACK);                 // Initiate LED Strip turned off
    for (size_t i = 0; i < NEOPixelsNUM; i++) NEOcolor_set (BLACK, i);  // Initiate NEO Pixel Strip turned off
    delay(200);
    //strcpy(Color,config.InitColor);   // for faster control of RGB, independent of WiFI/MQTT/system status
    //RGBcolor_set(Color);                 // Turning LED Strip to initial color (typicaly white...)
    //strcpy(LastColor,BLACK);          // and force publishing initial color after Wifi and MQQT initialization
}

  // Color commands to run on loop function.
void color_loop() {
      //if (String(Color) != String(LastColor)) if (color_set(Color)) mqtt_publish(mqtt_pathtele(), "Color", Color);
      switch (EFX) {
          case 0:
              break;
          case 1:
              color_Auto();
              break;
          case 2:
              color_Flash();
              break;
          case 3:
              color_Fade3();
              break;
          case 4:
              color_Fade7();
              break;
          case 5:
              color_Fade3();
              color_Flash();
              break;
          case 6:
              color_Fade7();
              color_Flash();
              break;
          case 7:
              color_rainbow();
              break;
          case 8:
              color_scan();
              break;
          default:
              RGBcolor_paint (RGB, GAIN);
              EFX = 0;
      }
      yield();
}
