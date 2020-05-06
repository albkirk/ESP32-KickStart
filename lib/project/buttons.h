#define BUTTONS_H true

void ICACHE_RAM_ATTR A_pressed ();
void ICACHE_RAM_ATTR B_pressed ();
void ICACHE_RAM_ATTR C_pressed ();
void ICACHE_RAM_ATTR T_Left_pressed ();
void ICACHE_RAM_ATTR T_Right_pressed ();

// **** Normal code definition here ...
#define BUT_A 35              // TTGo TS v1.0 Button A INPUT pin
#define BUT_B 34              // TTGo TS v1.0 Button B INPUT pin
#define BUT_C 39              // TTGo TS v1.0 Button C INPUT pin
#define T_Left T7             // Touch button Left  pin (GPIO27)
#define T_Right T9            // Touch button Right pin (GPIO32)
#define interval 50           // time interval window to ignore bouncing
bool A_STATUS = false;        // status of button A (true = pressed, false = released)
bool B_STATUS = false;        // status of button B (true = pressed, false = released)
bool C_STATUS = false;        // status of button C (true = pressed, false = released)
static volatile bool TL_STATUS = false;       // status of Touch Left  (true = pressed, false = released)
static volatile bool TR_STATUS = false;       // status of Touch Right (true = pressed, false = released)
int A_COUNT = 0;              // to count number of times button A is pressed within interval
int B_COUNT = 0;              // to count number of times button B is pressed within interval
int C_COUNT = 0;              // to count number of times button C is pressed within interval
static volatile int TL_COUNT = 0;             // to count number of times Touch Left is pressed within interval
static volatile int TR_COUNT = 0;             // to count number of times Touch Right is pressed within interval
uint32_t last_A = 0;               // timer var to avoid function call trigger due contact bounce
uint32_t last_B = 0;               // timer var to avoid function call trigger due contact bounce
uint32_t last_C = 0;               // timer var to avoid function call trigger due contact bounce
static volatile uint32_t last_TL = 0;              // timer var to avoid function call trigger due contact bounce
static volatile uint32_t last_TR = 0;              // timer var to avoid function call trigger due contact bounce
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;  // "critical section" Flag inside the callback


//byte TouchPins[] = { 04, 00, 02, 15, 13, 12, 14, 27, 33, 32 };  // ALL TOUCH PIN available!
//byte TouchPins[] = { T0, T1, T2, T3, T4, T5, T6, T7, T8, T9 };  // ALL TOUCH values available!
byte TouchPins[] =                               { T7,     T9 };  // Used on this project
#define NumTouchPins sizeof(TouchPins) / sizeof(*TouchPins)
uint16_t TouchThreshold[NumTouchPins] = {};
uint16_t TouchValMin = 10;                          // Minimum Touch Analog value measured
byte CalibrateTouch = 10;                           // Number of measurements it will do to calibrate the Touch threshold

// BUTTON and TOUCH related functions
void A_pressed () {
  detachInterrupt(BUT_A);                            // to avoid this function call be retriggered (or watch dog bites!!)
  int NOW = millis();
  if (NOW - last_A > interval / 2) {                 // respect minimum time between 2 consecutive function calls
      while (millis() - NOW < interval / 2) {};      // loop to allow button status be stable before reading it
      if (digitalRead(BUT_A) == LOW) {
          A_STATUS = true;
          if (NOW - last_A < 6 * interval) {
              A_COUNT += 1;
              Serial.println("Button A pressed " + String(A_COUNT) + "times!");
          }
          else {
              A_COUNT = 1;
              Serial.println("Button A pressed");
          };
      }
      else {
          A_STATUS = false;
          Serial.println("Button A released");
      };
  }
  last_A = NOW;
  attachInterrupt(BUT_A, A_pressed, CHANGE);
}

void B_pressed () {
  detachInterrupt(BUT_B);                            // to avoid this function call be retriggered (or watch dog bites!!)
  int NOW = millis();
  if (NOW - last_B > interval / 2) {                 // respect minimum time between 2 consecutive function calls
      while (millis() - NOW < interval / 2) {};      // loop to allow button status be stable before reading it
      if (digitalRead(BUT_B) == LOW) {
          B_STATUS = true;
          if (NOW - last_B < 6 * interval) {
              B_COUNT += 1;
              Serial.println("Button B pressed " + String(B_COUNT) + "times!");
          }
          else {
              B_COUNT = 1;
              Serial.println("Button B pressed");
          };
      }
      else {
          B_STATUS = false;
          Serial.println("Button B released");
      };
  }
  last_B = NOW;
  attachInterrupt(BUT_B, B_pressed, CHANGE);
}

void C_pressed () {
  detachInterrupt(BUT_C);                            // to avoid this function call be retriggered (or watch dog bites!!)
  int NOW = millis();
  if (NOW - last_C > interval / 2) {                 // respect minimum time between 2 consecutive function calls
      while (millis() - NOW < interval / 2) {};      // loop to allow button status be stable before reading it
      if (digitalRead(BUT_C) == LOW) {
          C_STATUS = true;
          if (NOW - last_C < 6 * interval) {
              C_COUNT += 1;
              Serial.println("Button C pressed " + String(C_COUNT) + "times!");
          }
          else {
              C_COUNT = 1;
              Serial.println("Button C pressed");
          };
      }
      else {
          C_STATUS = false;
          Serial.println("Button C released");
      };
  }
  last_C = NOW;
  attachInterrupt(BUT_C, C_pressed, CHANGE);
}

void T_Left_pressed () {
    //portENTER_CRITICAL(&mux);
    uint32_t NOW = millis();
    if (TL_STATUS == false) {
        while (millis() - NOW < interval  ) {};      // loop to allow button status be stable before reading it
        if ( touchRead(TouchPins[0]) <= TouchThreshold[0] ) {
            TL_STATUS = true;
            if (NOW - last_TL < 8 * interval) {
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
        while (millis() - NOW <  3 * interval) {};      // loop to allow button status be stable before reading it
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
        while ( millis() - NOW < interval ) {};      // loop to allow button status be stable before reading it
            if ( touchRead(TouchPins[1]) <= TouchThreshold[1] ) {
                TR_STATUS = true;
                if (NOW - last_TR < 8 * interval) {
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
        while (millis() - NOW <  3 * interval) {};      // loop to allow button status be stable before reading it
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

void buttons_setup() {
  if (BUT_A>=0) {
        pinMode(BUT_A, INPUT_PULLUP);
        attachInterrupt(BUT_A, A_pressed, CHANGE);
  }
  if (BUT_B>=0) {
        pinMode(BUT_B, INPUT_PULLUP);
        attachInterrupt(BUT_B, B_pressed, CHANGE);
  }
  if (BUT_C>=0) {
        pinMode(BUT_C, INPUT_PULLUP);
        attachInterrupt(BUT_C, C_pressed, CHANGE);
  }

  touch_calibrate();
  touchAttachInterrupt(TouchPins[0], &T_Left_pressed, TouchThreshold[0]);
  //touchAttachInterrupt(TouchPins[1], &T_Right_pressed, TouchThreshold[1]);

}
