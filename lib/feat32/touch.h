#define TOUCH_H true

void ICACHE_RAM_ATTR T_Left_pressed ();
void ICACHE_RAM_ATTR T_Right_pressed ();

// **** Normal code definition here ...
#define T_Left T7             // Touch button Left  pin (GPIO27)
#define T_Right T9            // Touch button Right pin (GPIO32)
#define Touch_Bounce_Time 25         // time interval window to ignore bouncing
#define Touch_Interval 500           // time interval window to count buttons pushes
static volatile bool TL_STATUS = false;       // status of Touch Left  (true = pressed, false = released)
static volatile bool TR_STATUS = false;       // status of Touch Right (true = pressed, false = released)
static volatile int TL_COUNT = 0;             // to count number of times Touch Left is pressed within interval
static volatile int TR_COUNT = 0;             // to count number of times Touch Right is pressed within interval
static volatile uint32_t last_TL = 0;              // timer var to avoid function call trigger due contact bounce
static volatile uint32_t last_TR = 0;              // timer var to avoid function call trigger due contact bounce
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;   // "critical section" Flag inside the callback


//byte TouchPins[] = { 04, 00, 02, 15, 13, 12, 14, 27, 33, 32 };  // ALL TOUCH PIN available!
//byte TouchPins[] = { T0, T1, T2, T3, T4, T5, T6, T7, T8, T9 };  // ALL TOUCH values available!
byte TouchPins[] =                               { T7,     T9 };  // Used on this project
#define NumTouchPins sizeof(TouchPins) / sizeof(*TouchPins)
uint16_t TouchThreshold[NumTouchPins] = {};
uint16_t TouchValMin = 10;                          // Minimum Touch Analog value measured
byte CalibrateTouch = 10;                           // Number of measurements it will do to calibrate the Touch threshold

// TOUCH related functions
void T_Left_pressed () {
    //portENTER_CRITICAL(&mux);
    unsigned long  NOW = millis();
    if (TL_STATUS == false) {
        while (millis() - NOW < Touch_Bounce_Time /2 ) {};      // loop to allow button status be stable before reading it
        if ( touchRead(TouchPins[0]) <= TouchThreshold[0] ) {
            TL_STATUS = true;
            if (NOW - last_TL < Touch_Interval) {
                TL_COUNT += 1;
                Serial.println("Touch Left pressed " + String(TL_COUNT) + "times!");
            }
            else {
                TL_COUNT = 1;
                Serial.println("Touch Left pressed");
            }
        }
    }
    else {
        while (millis() - NOW <  Touch_Bounce_Time) {};      // loop to allow button status be stable before reading it
        if ( touchRead(TouchPins[0]) > TouchThreshold[0]) {
            TL_STATUS = false;
            Serial.println("Touch Left released");
        }
    }
    last_TL = NOW;
    //portEXIT_CRITICAL(&mux);
}

void T_Right_pressed () {   // IRAM_ATTR
    //portENTER_CRITICAL(&mux);
    uint32_t NOW = millis();
    if (TR_STATUS == false) {
        while ( millis() - NOW < Touch_Bounce_Time /2 ) {};      // loop to allow button status be stable before reading it
            if ( touchRead(TouchPins[1]) <= TouchThreshold[1] ) {
                TR_STATUS = true;
                if (NOW - last_TR < Touch_Interval) {
                    TR_COUNT += 1;
                    Serial.println("Touch Right pressed " + String(TR_COUNT) + "times!");
                }
                else {
                    TR_COUNT = 1;
                    Serial.println("Touch Right pressed");
                }
            }
    }
    else {
        while (millis() - NOW <  Touch_Bounce_Time) {};      // loop to allow button status be stable before reading it
        if ( touchRead(TouchPins[1]) > TouchThreshold[1]) {
            TR_STATUS = false;
            Serial.println("Touch Right released");
        }
    }
    last_TR = NOW;
    //portEXIT_CRITICAL(&mux);
  }



void touch_calibrate() {
    Serial.println("Calibrating Touch....");
    for( byte n = 1; n < CalibrateTouch; ++n ) {
        for(byte idx = 0; idx < NumTouchPins; ++ idx ) TouchThreshold[idx] = TouchThreshold[idx] + touchRead(TouchPins[idx]);
    };
    for(byte idx = 0; idx < NumTouchPins; ++ idx ) {
        TouchThreshold[idx] = (TouchThreshold[idx] / CalibrateTouch - TouchValMin )* 1/4 + TouchValMin ;
        Serial.println("Touch: " + String(TouchPins[idx]) + " Threshold value: " + String(TouchThreshold[idx] ));
    };
}

void touch_setup() {
  touch_calibrate();
  touchAttachInterrupt(TouchPins[0], &T_Left_pressed, TouchThreshold[0]);
  //touchAttachInterrupt(TouchPins[1], &T_Right_pressed, TouchThreshold[1]);

}
