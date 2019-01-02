/* ****************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <images.h>

// TFT PINOUT
#define TFT_BACKLIGHT  7  // Display backlight pin (NOT CONFIRMED!!!)
#define TFT_CS        16
#define TFT_RST        9
#define TFT_DC        17
#define TFT_CLK       5   // set these to be whatever pins you like!
#define TFT_MOSI      23  // set these to be whatever pins you like!
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);
//Adafruit_ST7735 tft = Adafruit_ST7735(16, 17, 23, 5, 9); // CS,A0,SDA,CLK,RESET


#ifndef pi
#define pi 3.1415926
#endif

void testlines(uint16_t color) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y, color);
  }

  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y, color);
  }

  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
  }

  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
  }
}

void tft_text(String text, int color=ST7735_WHITE, uint8_t s=1, int x=0, int y=0, bool wraptext=true) {
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(s);
  tft.setTextWrap(wraptext);
  tft.print(text);
}

void tft_pixel(int x=0, int y=0, int color=ST7735_WHITE) {
    tft.drawPixel(tft.width()/2, tft.height()/2, ST7735_GREEN);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=radius; x < tft.width(); x+=radius*2) {
    for (int16_t y=radius; y < tft.height(); y+=radius*2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  tft.fillScreen(ST7735_BLACK);
  int color = 0xF800;
  int t;
  int w = tft.width()/2;
  int x = tft.height()-1;
  int y = 0;
  int z = tft.width();
  for(t = 0 ; t <= 15; t++) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testroundrects() {
  tft.fillScreen(ST7735_BLACK);
  int color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = tft.width()-2;
    int h = tft.height()-2;
    for(i = 0 ; i <= 16; i+=1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}

void mediabuttons() {
  // play
  tft.fillScreen(ST7735_BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, ST7735_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_RED);
  delay(500);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, ST7735_WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, ST7735_GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST7735_GREEN);
  delay(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_BLUE);
  delay(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, ST7735_RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST7735_RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_GREEN);
}

void tft_clrclock(uint16_t bgcolor=ST7735_BLACK, int x=2, int y=56) {
      String text="";
      if (LastDateTime.hour <=9) text = String("0") + String(LastDateTime.hour);
      else text = String(LastDateTime.hour);
      tft_text(text,        bgcolor, 4, x,      y);
      tft_text(String(':'), bgcolor, 4, x + 40, y);

      if (LastDateTime.minute <=9) text = String("0") + String(LastDateTime.minute);
      else text = String(LastDateTime.minute);
      tft_text(text,      bgcolor, 4, x + 55, y);

      if (LastDateTime.second <=9) text = String("0") + String(LastDateTime.second);
      else text = String(LastDateTime.second);
      tft_text(text,      bgcolor, 2, x + 102, y);
}


void tft_drawclock(uint16_t color=ST7735_YELLOW, uint16_t bgcolor=ST7735_BLACK, int x=2, int y=56) {
  curDateTime();
  String text="";
  if (LastDateTime.hour != DateTime.hour) {
      if (LastDateTime.hour <=9) text = String("0") + String(LastDateTime.hour);
      else text = String(LastDateTime.hour);
      tft_text(text,      bgcolor, 4, x,      y);
      if (DateTime.hour <=9)     text = String("0") + String(DateTime.hour);
      else text = String(DateTime.hour);
      tft_text(text,        color, 4, x,      y);
      tft_text(String(':'), color, 4, x + 40, y);
  }
  if (LastDateTime.minute != DateTime.minute) {
      if (LastDateTime.minute <=9) text = String("0") + String(LastDateTime.minute);
      else text = String(LastDateTime.minute);
      tft_text(text,      bgcolor, 4, x + 55, y);
      if (DateTime.minute <=9)     text = String("0") + String(DateTime.minute);
      else text = String(DateTime.minute);
      tft_text(text,        color, 4, x + 55, y);
      //tft_text(String(':'), color, 2, x + 86, y);
  }
  if (LastDateTime.second != DateTime.second) {
      if (LastDateTime.second <=9) text = String("0") + String(LastDateTime.second);
      else text = String(LastDateTime.second);
      tft_text(text,      bgcolor, 2, x + 102, y);
      if (DateTime.second <=9)     text = String("0") + String(DateTime.second);
      else text = String(DateTime.second);
      tft_text(text,        color, 2, x + 102, y);
  }
  LastDateTime = DateTime;
  //tft_text(String(), color, 2, x+30, y);
  //tft_text(String(), color, 2, x+60, y);
}


void tft_clrdate(uint16_t bgcolor=ST7735_BLACK, int x=7, int y=100) {
  //curDateTime();  //Skipped as tft_drawclock() function should run much more often than this one.
      String text="";
      text = WeekDays[LastDateTime.wday];
      tft_text(text,        bgcolor, 2, x,      y);
      tft_text(String(','), bgcolor, 2, x + 35, y);

      if (LastDateTime.day <=9) text = String("0") + String(LastDateTime.day);
      else text = String(LastDateTime.day);
      tft_text(text,        bgcolor, 2, x + 50, y);
      tft_text(String('/'), bgcolor, 2, x + 74, y);

      if (LastDateTime.month <=9) text = String("0") + String(LastDateTime.month);
      else text = String(LastDateTime.month);
      tft_text(text,      bgcolor, 2, x + 86, y);
}

void tft_drawdate(uint16_t color=ST7735_WHITE, uint16_t bgcolor=ST7735_BLACK, int x=7, int y=100) {
  //curDateTime();  //Skipped as tft_drawclock() function should run much more often than this one.
  String text="";
  if (LastDateTime.wday != DateTime.wday) {
      text = WeekDays[LastDateTime.wday];
      tft_text(text,      bgcolor, 2, x,      y);
      text = WeekDays[DateTime.wday];
      tft_text(text,        color, 2, x,      y);
      tft_text(String(','), color, 2, x + 35, y);
  }
  if (LastDateTime.day != DateTime.day) {
      if (LastDateTime.day <=9) text = String("0") + String(LastDateTime.day);
      else text = String(LastDateTime.day);
      tft_text(text,      bgcolor, 2, x + 50,      y);
      if (DateTime.day <=9)     text = String("0") + String(DateTime.day);
      else text = String(DateTime.day);
      tft_text(text,        color, 2, x + 50,      y);
      tft_text(String('/'), color, 2, x + 74, y);
  }
  if (LastDateTime.month != DateTime.month) {
      if (LastDateTime.month <=9) text = String("0") + String(LastDateTime.month);
      else text = String(LastDateTime.month);
      tft_text(text,      bgcolor, 2, x + 86, y);
      if (DateTime.month <=9)     text = String("0") + String(DateTime.month);
      else text = String(DateTime.month);
      tft_text(text,        color, 2, x + 86, y);
      //tft_text(String('/'), color, 2, x + 61, y);
  }
  /*if (LastDateTime.year != DateTime.year) {
    if (LastDateTime.year <=9) text = String("0") + String(LastDateTime.year);
    else text = String(LastDateTime.year);
    tft_text(text,      bgcolor, 2, x + 73, y);
    if (DateTime.year <=9)     text = String("0") + String(DateTime.year);
    else text = String(DateTime.year);
    tft_text(text,        color, 2, x + 73, y);
  }*/
}


void tft_setup() {
    // Use this initializer if you're using a 1.8" TFT
    //tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

    // Use this initializer (uncomment) if you're using a 1.44" TFT
    tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab
    Serial.println("TFT Initialized");

    long deltatime = millis();
    tft.fillScreen(ST7735_BLACK);
    deltatime = millis() - deltatime;

    tft.setRotation(rotate);
    /*
    tft.setTextColor(ST7735_RED);
    tft.setTextSize(0);
    tft.println("time: " + String(deltatime));
    tft.println("F Rate: " + String(1000/deltatime));
    delay(5000);
    */
    tft.drawBitmap(0,0,bell,16,16,ST7735_ORANGE);
    //tft.drawBitmap(0,0,semiCircle, 50, 25, ST7735_WHITE);
    tft.drawBitmap(20,0,wifi_icon, 20, 15, ST7735_WHITE);
    tft.drawBitmap(40,0,user_icon, 15, 15, ST7735_WHITE);
    tft.drawBitmap(100,0,battery_icon, 20, 15, ST7735_GREEN);
}

void tft_clock() {
    if (((millis() - RefMillis)%1000) < 20) {
        tft_drawdate();
        tft_drawclock();
    }
}
