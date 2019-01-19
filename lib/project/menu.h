
//  - - -  Constants
//static const String menu_main[] = {"Clock", "Alarm", "ZEN", "Shades", "System"};
static const String menu_main[] = {"Clock", "Alarm", "Sounds", "Lights", "Shades", "System"};
static const String menu_zen[] =    {"Florest",     "Beach",    "Night",     "Raibow",    "Sunset"};
//static const String sounds[] = {"Birds",       "Ocean",    "Crickets",  "Xilophone", "Waterfall"};
static const String lights[] = {"LIGHT_GREEN", "SKY_BLUE", "DEEP_BLUE", "PURPLE",    "ORANGE"};

// static const String SYSmenu[] = {"Sounds", "Lights", "IoT"};


//  - - -  Variables  - - -
byte MENU = 0;
byte Last_MENU = (sizeof(menu_main)/sizeof(*menu_main));
uint32_t MENU_LastTime = 0;              // Last MENU selection time stamp
unsigned int Backto_MENU = 120;               // Timeout to return to Main (clock) Menu
bool Menu_Next = false;                       // Aux flag to jumo to next menu
bool Menu_1stRun = false;                     // Aux flag to run a pice of code just once
int delta = 0;                                // Aux var with the value to be added. Used on functions: set_alarm(),...

byte ZEN = 0;
byte SOUNDs = 0;
byte LIGHTs = 0;

//  - - - AUX libraries
#include <mn_alarm.h>
//#include <mn_zen.h>
#include <mn_sounds.h>
#include <mn_lights.h>
#include <mn_shades.h>
#include <mn_clock.h>  /// this must be the last one as it calls functions from previous libs
//  - - -  Functions  - - -
void batt_icon_update() {
    tft_text((String((int)Batt_Level)+"%"),BGColor, 1, 104, 16);
    tft_text((String(voltage, 2)+"v"),BGColor, 1, 98, 26);
    Batt_Level = getVoltage();
    tft_text((String((int)Batt_Level)+"%"),ST7735_WHITE, 1, 104, 16);
    tft_text((String(voltage, 2)+"v"),ST7735_WHITE, 1, 98, 26);
    if (Batt_Level > 75)  {
        tft.drawBitmap(110, 4,battery_icon_1_4, 16, 8, ST7735_WHITE);
        tft.fillRect(113, 6, 2, 4, ST7735_WHITE);
        tft.fillRect(117, 6, 2, 4, ST7735_WHITE);
        tft.fillRect(121, 6, 2, 4, ST7735_WHITE);
    }
    else if (Batt_Level > 50 && Batt_Level <= 75) {
        tft.drawBitmap(110, 4,battery_icon_1_4, 16, 8, ST7735_WHITE);
        tft.fillRect(113, 6, 2, 4, ST7735_WHITE);
        tft.fillRect(117, 6, 2, 4, ST7735_WHITE);
        tft.fillRect(121, 6, 2, 4, BGColor);
    }
    else if (Batt_Level > 25 && Batt_Level <= 50) {
      tft.drawBitmap(110, 4,battery_icon_1_4, 16, 8, ST7735_WHITE);
      tft.fillRect(113, 6, 2, 4, ST7735_WHITE);
      tft.fillRect(117, 6, 2, 4, BGColor);
      tft.fillRect(121, 6, 2, 4, BGColor);
    }
    else if (Batt_Level <= 25)  {
        tft.drawBitmap(110, 4,battery_icon_1_4, 16, 8, ST7735_RED);
        tft.fillRect(113, 6, 2, 4, BGColor);
        tft.fillRect(117, 6, 2, 4, BGColor);
        tft.fillRect(121, 6, 2, 4, BGColor);
        player_beepdn(2);
    }
    //LOW_Batt_check();
}

void loop_icons() {
  //  -- WiFI Icon --
    if ( WIFI_state != Last_WIFI_state ) {
        if ( WIFI_state == WL_CONNECTED ) tft.drawBitmap(20, 0, wifi_icon, 16, 16, ST7735_WHITE);
        else tft.drawBitmap(20, 0, wifi_icon, 16, 16, BGColor);
        Last_WIFI_state = WIFI_state;
    };
    //tft.drawBitmap(20, 0, wifi_OFF_icon, 16, 16, ST7735_GREEN);


  // -- ALARM Icon --
    if ( config.Alarm_State != Last_Alarm_State ) {
        if (config.Alarm_State) tft.drawBitmap(0,0,bell_icon,16,16,MainColor);
        else tft.drawBitmap(0,0,bell_icon,16,16,BGColor);
        Last_Alarm_State = config.Alarm_State;
    };

  // battery Icon
    if (((millis() - RefMillis)%30000) < 20) batt_icon_update();

  // Touch Button
    if (TL_STATUS == true ) {      // Were Touch Buttons pressed?
        tft_text(("Touched!"),ST7735_GREEN, 1, 40, 2);
        delay(250);
        tft_text(("Touched!"),BGColor, 1, 40, 2);
    }
}


void loop_system() {
}


void menu_setup() {
  Last_Alarm_State = !config.Alarm_State;             // To force the bell icon update
  batt_icon_update();                                 // To force the Battery icon update
}

void menu_loop() {
    // MENU handling
    if (Last_MENU != MENU) {
        switch(Last_MENU) {   // actions to execute before Leaving the current menu. typically, to clean the screen...
            case 0:     // Clock
                tft_drawtime(LastDateTime, BGColor);  // Clear clock
                tft_drawdate(LastDateTime, BGColor);  // Clear date
                break;
            case 1:     // Alarm
                tft_drawalarm(config.AlarmDateTime, BGColor);
                break;
            case 2:     // Sounds
                tft_drawprevious(BGColor);
                tft_drawplay(0, BGColor);
                tft_drawnext(BGColor);
                tft_drawsound(SOUNDs, BGColor);
                tft_drawvolume(config.Volume, BGColor);
                break;
            case 3:     // Lights
                tft_drawEFX(EFX, BGColor);
                for (size_t i = 0; i < NEOPixelsNUM; i++) NEOcolor_set (BLACK, i);
                break;
            case 4:     // Shades
                    tft_drawshades(SHADES, BGColor);
                    break;
        }
        switch(MENU) {  // actions to execute whenn moving to current menu. typically, draw the full image.
            case 0:     // Clock
                curDateTime();
                tft_drawtime(DateTime, MainColor);
                tft_drawdate(DateTime, MainColor);
                LastDateTime = DateTime;
                break;
            case 1:     // Alarm
                tft_drawalarm(config.AlarmDateTime, MainColor);
                break;
            case 2:     // Sounds
                tft_drawprevious(MainColor);
                tft_drawplay(0, MainColor);
                tft_drawnext(MainColor);
                tft_drawsound(SOUNDs, MainColor);
                tft_drawvolume(config.Volume, MainColor);
                break;
            case 3:     // Lights
                tft_drawEFX(EFX, MainColor);
                for (size_t i = 0; i < NEOPixelsNUM; i++) NEOcolor_set (BLACK, i);
                break;
            case 4:     // Shades
                tft_drawshades(SHADES, MainColor);
                break;
        }
        //telnet_print("Last Menu: " + menu_main[Last_MENU]);
        //telnet_println("\tCurrent Menu: " + menu_main[MENU]);
        tft_text(menu_main[Last_MENU], BGColor, 1, Last_MENU * 18, 20);  // Clear Menu
        tft_text(menu_main[MENU], MainColor, 1, MENU * 18, 20);          // write Menu
        Last_MENU = MENU;
        MENU_LastTime = millis();
    }
    // Timeout to clear sub-menu flags and return to Main (clock) Menu

    if ( millis() - MENU_LastTime > (Backto_MENU * 1000)) {
        Alarm_Set = false;
        MENU = 0;
    }

    switch(MENU) {
        case 0:     // Clock
                loop_clock();
                break;
        case 1:     // alarm
                loop_alarm();
                break;

        case 2:
                loop_sounds();
                break;
        case 3:
                loop_lights();
                break;
        case 4:
                loop_shades();
                break;
        /*
        case 5:
                loop_system();
                break;
        */
        default :
                if(A_COUNT == 1 && !A_STATUS && (millis() - last_A > 6 * interval)) {
                    MENU = (MENU + 1)%(sizeof(menu_main)/sizeof(*menu_main));
                    telnet_println("Menu: " + menu_main[MENU]);
                    A_COUNT = 0;
                }
                if(A_COUNT == 2 && !A_STATUS && (millis() - last_A > 6 * interval)) {
                    MENU = 0;
                    A_COUNT = 0;
                }
    }

    // ICONs handling
    loop_icons();
}
