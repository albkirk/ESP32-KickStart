// **** Normal code definition here ...
#define BUT_A 35                                // Button A INPUT pin
#define BUT_B 34                                // Button B INPUT pin
#define BUT_C 39                                // Button C INPUT pin
#define interval 50             // time interval window to ignore bouncing
bool A_STATUS = false;      // status of button A (true = pressed, false = released)
bool B_STATUS = false;      // status of button B (true = pressed, false = released)
bool C_STATUS = false;      // status of button C (true = pressed, false = released)
int A_COUNT = 0;               // to count number of times button A is pressed within interval
int B_COUNT = 0;               // to count number of times button B is pressed within interval
int C_COUNT = 0;               // to count number of times button C is pressed within interval
int STEPs = 0;                  // to count axel number of steps (1/16 turn)
int last_another_step = 0;      // timer var to avoid function call trigger due contact bounce
int last_A = 0;                // timer var to avoid function call trigger due contact bounce
int last_B = 0;                // timer var to avoid function call trigger due contact bounce
int last_C = 0;                // timer var to avoid function call trigger due contact bounce


// BUTTON and REED related functions
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


void buttons_setup() {
  pinMode(BUT_A, INPUT_PULLUP);
  pinMode(BUT_B, INPUT_PULLUP);
  pinMode(BUT_C, INPUT_PULLUP);
  attachInterrupt(BUT_A, A_pressed, CHANGE);
  attachInterrupt(BUT_B, B_pressed, CHANGE);
  attachInterrupt(BUT_C, C_pressed, CHANGE);
}
