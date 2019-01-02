//  - - - libraries

//  - - -  Constants
static const String menu[] = {"Clock", "Alarm", "Lights", "Sounds", "System"};


//  - - -  Variables  - - -
byte MENU = 0;
byte Last_MENU = (sizeof(menu)/sizeof(*menu));

//  - - -  Functions  - - -
void menu_setup() {

}

void menu_loop() {
  if(A_COUNT == 1) {
      if (MENU <= 0) MENU = (sizeof(menu)/sizeof(*menu))-1;
      else MENU --;
      telnet_println("Menu: " + menu[MENU]);
      A_COUNT = 0;
  }
  if(B_COUNT == 1) {
      player_tone(B5, 500);
      B_COUNT = 0;
  }
  if(C_COUNT == 1) {
      if (MENU >= (sizeof(menu)/sizeof(*menu))-1) MENU = 0;
      else MENU ++;
      telnet_println("Menu: " + menu[MENU]);
      C_COUNT = 0;
  }

    if (Last_MENU != MENU) {
        switch(Last_MENU) {
            case 0:
                tft_clrclock();
                tft_clrdate();
                telnet_println("case 0");
                break;
            case 1:
                break;
        }
        telnet_print("Last Menu: " + menu[Last_MENU]);
        telnet_println("\tCurrent Menu: " + menu[MENU]);
        Last_MENU = MENU;
    }

        switch(MENU) {
            case 0:
                tft_clock();
                break;
            case 1:
                break;
        }
}
