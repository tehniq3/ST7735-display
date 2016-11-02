/*
 * * original from http://www.instructables.com/id/Arduino-TFT-display-and-font-library/
 * some changes by Nicu FLORICA (niq_ro) from http://www.tehnic.go.ro
 * 
  An example analogue clock using a TFT LCD screen to show the time
 use of some of the drawing commands with the ST7735 library.
 For a more accurate clock, it use the RTClib library.
 
 This examples uses the hardware SPI only. Non-hardware SPI
 is just too slow (~8 times slower!)
 
 Gilchrist 6/2/2014 1.0
 Updated by Bodmer, using library from https://github.com/Bodmer/TFT_ST7735
 */

#include <TFT_ST7735.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_ST7735 tft = TFT_ST7735();  // Invoke library, pins defined in User_Setup.h

#define ST7735_GREY 0xBDF7

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg=0, mdeg=0, hdeg=0;
uint16_t osx=64, osy=64, omx=64, omy=64, ohx=64, ohy=64;  // Saved H, M, S x & y coords
uint16_t x0=0, x1=0, y0=0, y1=0;
uint32_t targetTime = 0;                    // for next 1 second timeout

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

//uint8_t hh=conv2d(__TIME__), mm=conv2d(__TIME__+3), ss=conv2d(__TIME__+6);  // Get H, M, S from compile time
boolean initial = 1;

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
  tft.setRotation(0);
  tft.fillScreen(ST7735_GREY);
  tft.setTextColor(ST7735_GREEN, ST7735_GREY);  // Adding a black background colour erases previous text automatically

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
  
  // Draw clock face
  tft.fillCircle(64, 64, 61, ST7735_BLUE);
  tft.fillCircle(64, 64, 57, ST7735_BLACK);

  // Draw 12 lines
  for(int i = 0; i<360; i+= 30) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx*57+64;
    y0 = sy*57+64;
    x1 = sx*50+64;
    y1 = sy*50+64;

    tft.drawLine(x0, y0, x1, y1, ST7735_BLUE);
  }

  // Draw 60 dots
  for(int i = 0; i<360; i+= 6) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx*53+64;
    y0 = sy*53+64;
    
    tft.drawPixel(x0, y0, ST7735_BLUE);
    if(i==0 || i==180) tft.fillCircle(x0, y0, 1, ST7735_CYAN);
    if(i==0 || i==180) tft.fillCircle(x0+1, y0, 1, ST7735_CYAN);
    if(i==90 || i==270) tft.fillCircle(x0, y0, 1, ST7735_CYAN);
    if(i==90 || i==270) tft.fillCircle(x0+1, y0, 1, ST7735_CYAN);
  }

  tft.fillCircle(65, 65, 3, ST7735_RED);
/*
  // Draw text at position 64,125 using fonts 4
  // Only font numbers 2,4,6,7 are valid. Font 6 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : . a p m
  // Font 7 is a 7 segment font and only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : .
  tft.drawCentreString("Time flies",64,130,4);
*/
  targetTime = millis() + 1000; 
}

void loop() {
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

    // Pre-compute hand degrees, x & y coords for a fast screen update
    sdeg = ss*6;                  // 0-59 -> 0-354
    mdeg = mm*6+sdeg*0.01666667;  // 0-59 -> 0-360 - includes seconds
    hdeg = hh*30+mdeg*0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds
    hx = cos((hdeg-90)*0.0174532925);    
    hy = sin((hdeg-90)*0.0174532925);
    mx = cos((mdeg-90)*0.0174532925);    
    my = sin((mdeg-90)*0.0174532925);
    sx = cos((sdeg-90)*0.0174532925);    
    sy = sin((sdeg-90)*0.0174532925);

    if (ss==0 || initial) {
      initial = 0;
      // Erase hour and minute hand positions every minute
      tft.drawLine(ohx, ohy, 65, 65, ST7735_BLACK);
      ohx = hx*33+65;    
      ohy = hy*33+65;
      tft.drawLine(omx, omy, 65, 65, ST7735_BLACK);
      omx = mx*44+65;    
      omy = my*44+65;
    }

      // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
      tft.drawLine(osx, osy, 65, 65, ST7735_BLACK);
      tft.drawLine(ohx, ohy, 65, 65, ST7735_WHITE);
      tft.drawLine(omx, omy, 65, 65, ST7735_WHITE);
      osx = sx*47+65;    
      osy = sy*47+65;
      tft.drawLine(osx, osy, 65, 65, ST7735_RED);

    tft.fillCircle(65, 65, 3, ST7735_RED);
  }

if (temperatura0 != temperatura) 
    { 
   for (int y=130; y < 160; y+=1)
   {
   tft.drawFastHLine(0, y, 128, ST7735_GREY);
   }
   tft.setTextSize(3);
   tft.setTextColor(TFT_BLACK);
   tft.setCursor (10, 130);
   if (temperatura <10) tft.print(" ");
   tft.print(temperatura,1);
   tft.print(" C");
   tft.setTextSize(2);       
   tft.setCursor (85, 130);
   tft.print("O");
   tft.setTextSize(1);      
   temperatura0 = temperatura;
    }

tft.fillCircle(65, 65, 3, ST7735_RED);
  
}  // end main loop

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

