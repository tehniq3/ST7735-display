/*
 * original from http://www.instructables.com/id/Arduino-TFT-display-and-font-library/
 * some changes by Nicu FLORICA (niq_ro) from http://www.tehnic.go.ro
 * 
 An example digital clock using a TFT LCD screen to show the time.
 Demonstrates use of the font printing routines. (Time updates but date does not.)
 
 Now it use RTClib library bu Nicu FLORICA
 
 This examples uses the hardware SPI only. Non-hardware SPI
 is just too slow (~8 times slower!)
 
 Based on clock sketch by Gilchrist 6/2/2014 1.0
 Updated by Bodmer, using library from https://github.com/Bodmer/TFT_ST7735
   
A few colour codes:
 
code	color
0x0000	Black
0xFFFF	White
0xBDF7	Light Gray
0x7BEF	Dark Gray
0xF800	Red
0xFFE0	Yellow
0xFBE0	Orange
0x79E0	Brown
0x7E0	Green
0x7FF	Cyan
0x1F	Blue
0xF81F	Pink

 */

#include <TFT_ST7735.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_ST7735 tft = TFT_ST7735();  // Invoke library, pins defined in User_Setup.h
// see User_Setup.h: RST --> D7, CS --> D9, D/C --> D8, DIN --> D11, CLK --> D13


uint32_t targetTime = 0;       // for next 1 second timeout

byte omm = 99;
boolean initial = 1;
byte xcolon = 0;
unsigned int colour = 0;

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

//uint8_t hh=conv2d(__TIME__), mm=conv2d(__TIME__+3), ss=conv2d(__TIME__+6);  // Get H, M, S from compile time
byte jos = 10;

#include <Wire.h>
#include "RTClib.h"
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int hh, mm, ss;
int yy, ll, dd, zz;
#define DS3231_I2C_ADDRESS 104
byte tMSB, tLSB;
float temp3231;
float temperatura, temperatura0;

void setup(void) {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);

  tft.setTextColor(ST7735_YELLOW, ST7735_BLACK); // Note: the new fonts do not draw the background colour

 Serial.begin(9600);

  delay(3000); // wait for console opening

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  targetTime = millis() + 1000; 
}

void loop() {

tft.setTextSize(1);
  
  if (targetTime < millis()) {
    targetTime = millis()+1000;
DateTime now = rtc.now();
/*    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
*/
 
hh = now.hour(), DEC;
mm = now.minute(), DEC;
ss = now.second(), DEC;

yy = now.year(), DEC;
ll = now.month(), DEC;
dd = now.day(), DEC;
//zz = daysOfTheWeek[now.dayOfTheWeek()];

temperatura = get3231Temp();

    tft.setTextSize(1);
    if (ss==0 || initial) {
      initial = 0;
      tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
      tft.setCursor (15, 52+jos);
  //    tft.print(__DATE__); // This uses the standard ADAFruit small font
      tft.print(daysOfTheWeek[now.dayOfTheWeek()]);
      tft.print(" - ");
      tft.print(ll);
      tft.print("/");
      tft.print(dd);
      tft.print("/");
      tft.print(yy);

if (temperatura0 != temperatura) 
    { 
   for (int y=85; y < tft.height(); y+=1) {
   tft.drawFastHLine(0, y, tft.width(), ST7735_BLACK);
   }
int temperatura1 = temperatura;
float temperatura2 = 100*(temperatura - temperatura1);
   tft.setTextSize(3);
   tft.setTextColor(TFT_YELLOW);
   tft.setCursor (30, 85+jos);
   if (temperatura <10) tft.print(" ");
 //  tft.print(temperatura);
   tft.print(temperatura1);
   tft.print(".");
   tft.setCursor (85, 90+jos);
   tft.setTextSize(2); 
   if (temperatura2 == 0) tft.print("00");  
   else tft.print(temperatura2,0);
   tft.setCursor (124, 85+jos);
   tft.setTextSize(3);      
   tft.print("C");
   tft.setTextSize(2);       
   tft.setCursor (110, 84+jos);
   tft.print("O");
   tft.setTextSize(1);      
   temperatura0 = temperatura;
    }
     }

    // Update digital time
    byte xpos = 6;
    byte ypos = 0+jos;
    if (omm != mm) { // Only redraw every minute to minimise flicker
      // Uncomment ONE of the next 2 lines, using the ghost image demonstrates text overlay as time is drawn over it
      //tft.setTextColor(ST7735_RED, ST7735_BLACK);  // Leave a 7 segment ghost image, comment out next line!
      tft.setTextColor(ST7735_BLACK, ST7735_BLACK); // Set font colour to back to wipe image
      // Font 7 is to show a pseudo 7 segment display.
      // Font 7 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 0 : .
      tft.drawString("88:88",xpos,ypos,7); // Overwrite the text to clear it
      tft.setTextColor(ST7735_RED, ST7735_BLACK); // Red
      omm = mm;

      if (hh<10) xpos+= tft.drawChar('0',xpos,ypos,7);
      xpos+= tft.drawNumber(hh,xpos,ypos,7);
      xcolon=xpos;
      xpos+= tft.drawChar(':',xpos,ypos,7);
      if (mm<10) xpos+= tft.drawChar('0',xpos,ypos,7);
      tft.drawNumber(mm,xpos,ypos,7);
    }

    if (ss%2) { // Flash the colon
      tft.setTextColor(0x39C4, ST7735_BLACK);
      xpos+= tft.drawChar(':',xcolon,ypos,7);
      //tft.setTextColor(0xFBE0, ST7735_BLACK);
        tft.setTextColor(ST7735_RED, ST7735_BLACK);
    }
    else {
      tft.drawChar(':',xcolon,ypos,7);
      colour = random(0xFFFF);
      // Erase the text with a rectangle
      tft.fillRect (0, 64+jos, 160, 20, ST7735_BLACK);
    }
  }
}

float get3231Temp() // from http://www.goodliffe.org.uk/arduino/rtc_ds3231.php
{
  //temp registers (11h-12h) get updated automatically every 64s
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x11);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 2);
 
  if(Wire.available()) {
    tMSB = Wire.read(); //2's complement int portion
    tLSB = Wire.read(); //fraction portion
   
    temp3231 = (tMSB & B01111111); //do 2's math on Tmsb
    temp3231 += ( (tLSB >> 6) * 0.25 ); //only care about bits 7 & 8
  }
  else {
    //oh noes, no data!
  }
   
  return temp3231;
}
