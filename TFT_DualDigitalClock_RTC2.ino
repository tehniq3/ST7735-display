/*
 * original from http://www.instructables.com/id/Arduino-TFT-display-and-font-library/
 * some changes by Nicu FLORICA (niq_ro) from http://www.tehnic.go.ro
 * ver.1a- add flashing second signs
 * ver.1b - add temperature
 * ver.1c - add buttons for change the data and hours & eliminate RTC library
 * ver.2 - put dual clock (GMT/UTC and local time)
 * 
 An example digital clock using a TFT LCD screen to show the time.
 Demonstrates use of the font printing routines. (Time updates but date does not.)
 
 Now it use RTClib library bu Nicu FLORICA
 
 This examples uses the hardware SPI only. Non-hardware SPI
 is just too slow (~8 times slower!)
 
 Based on clock sketch by Gilchrist 6/2/2014 1.0
 Updated by Bodmer, using library from https://github.com/Bodmer/TFT_ST7735
   
A few colour codes:
 
code  color
0x0000  Black
0xFFFF  White
0xBDF7  Light Gray
0x7BEF  Dark Gray
0xF800  Red
0xFFE0  Yellow
0xFBE0  Orange
0x79E0  Brown
0x7E0 Green
0x7FF Cyan
0x1F  Blue
0xF81F  Pink
 */

#include <TFT_ST7735.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include "EEPROM.h"

TFT_ST7735 tft = TFT_ST7735();  // Invoke library, pins defined in User_Setup.h
// see User_Setup.h: RST --> D7, CS --> D9, D/C --> D8, DIN --> D11, CLK --> D13


uint32_t targetTime = 0;       // for next 1 second timeout

byte omm = 99;
boolean initial = 1;
byte xcolon = 0;
byte xcolon2 = 0;
unsigned int colour = 0;

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

#include <Wire.h>
// char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};  // english version
char daysOfTheWeek[7][12] = {"Duminica", "Luni", "Marti", "Miercuri", "Joi", "Vineri", "Sambata"};  // romanian version
byte hh, mm, ss;
byte yy, ll, dd, zz;

#define DS3231_I2C_ADDRESS 104
byte tMSB, tLSB;
float temp3231;
float temperatura, temperatura0;


#define meniu 2 
#define minus 3
#define plus 4
int nivel = 0;   // if is 0 - clock
                 // if is 1 - hour adjust
                 // if is 2 - minute adjust
                 // if is 3 - day of week adjust
                 // etc
byte jos = 10;
//byte hh1, mm1, zz1, dd1, ll1, yy1;   
int hh1, mm1, zz1, dd1, ll1, yy1;   
byte maxday;

int hh2;
int hh3;
byte hh4;
byte semn;

void setup(void) {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);

  tft.setTextColor(ST7735_YELLOW, ST7735_BLACK); // Note: the new fonts do not draw the background colour

pinMode(meniu, INPUT); 
pinMode(plus, INPUT); 
pinMode(minus, INPUT); 
digitalWrite(meniu, HIGH);  // put inputs in high state (when push is to ground -> low state)
digitalWrite(plus, HIGH);
digitalWrite(minus, HIGH);

 Serial.begin(9600);
 Wire.begin();
// setDS3231time(30, 42, 21, 4, 26, 11, 1);
  targetTime = millis() + 1000; 

hh4 = EEPROM.read(100);
semn = EEPROM.read(101);

if ((hh4 < 0) || (hh4 > 12)) hh4 = 2;
if ((semn < 0) || (semn > 1)) semn = 1;   // if 1 -> +, 0 -> -

if (semn == 1) hh3 = hh4;
if (semn == 0) hh3 = -hh4;

hh2 = hh - hh3;  
}

void loop() {

if (nivel == 0)
{
if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  delay(500);
  hh1=hh;
//  tft.fillScreen(ST7735_BLACK);
 sterg();
  }
tft.setTextSize(1);
  if (targetTime < millis()) {
    targetTime = millis()+1000;

// readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,&year);
 readDS3231time(&ss, &mm, &hh, &zz, &dd, &ll,&yy);
hh2 = hh - hh3;
if (hh2 > 23) hh2 = hh2-24;
if (hh2 < 0) hh2 = 24+hh2;

//temperatura = get3231Temp();

    tft.setTextSize(1);
    if (ss==0 || initial) {

   for (int y=52+jos; y < 60+jos; y+=1) 
   {
   tft.drawFastHLine(0, y, tft.width(), ST7735_BLACK);
   }
      
      initial = 0;
      tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
      tft.setCursor (15, 52+jos);
      tft.print(daysOfTheWeek[zz]);
      tft.setCursor (90, 52+jos);

      if (dd <10) tft.print("0");
      tft.print(dd);
      tft.print(".");
      if (ll <10) tft.print("0");
      tft.print(ll);
      tft.print(".");
      tft.print(yy+2000);
/*
if (temperatura0 != temperatura) 
    {
  if (digitalRead(meniu) == LOW) 
  {
  nivel = nivel+1;
  delay(500);
  hh1=hh;
  tft.fillScreen(ST7735_BLACK);    
    }
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
*/
     }

    // Update digital time
    byte xpos = 16;
    byte ypos = 0+jos;
    byte xpos2 = 16;
    byte ypos2 = 65+jos;

      tft.setTextColor(ST7735_BLUE, ST7735_BLACK);
      tft.setCursor (0, 0);
      tft.print("GMT");
/*
      tft.setCursor (0, ypos+2);
      tft.print("G");
      tft.setCursor (0, ypos+10);
      tft.print("M");
      tft.setCursor (0, ypos+18);
      tft.print("T");
      tft.setCursor (0, ypos+34);
*/
      if (hh3 == 0) tft.print("=");
      if (hh3 > 0) tft.print("+");
      tft.print(hh3);
  

      tft.setCursor (0, ypos2+2);
      tft.print("G");
      tft.setCursor (0, ypos2+10);
      tft.print("M");
      tft.setCursor (0, ypos2+18);
      tft.print("T");


    
    if (omm != mm) { // Only redraw every minute to minimise flicker
    if (digitalRead(meniu) == LOW) {
    nivel = nivel+1;
    tft.fillScreen(ST7735_BLACK);
    delay(500);
    tft.fillScreen(ST7735_BLACK);
    }
      // Uncomment ONE of the next 2 lines, using the ghost image demonstrates text overlay as time is drawn over it
      //tft.setTextColor(ST7735_RED, ST7735_BLACK);  // Leave a 7 segment ghost image, comment out next line!
      tft.setTextColor(ST7735_BLACK, ST7735_BLACK); // Set font colour to back to wipe image
      // Font 7 is to show a pseudo 7 segment display.
      // Font 7 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 0 : .
      tft.drawString("88:88",xpos,ypos,7); // Overwrite the text to clear it
      tft.drawString("88:88",xpos2,ypos2,7); // Overwrite the text to clear it
      tft.setTextColor(ST7735_RED, ST7735_BLACK); // Red
      omm = mm;

      if (hh<10) xpos+= tft.drawChar('0',xpos,ypos,7);
      xpos+= tft.drawNumber(hh,xpos,ypos,7);
      xcolon=xpos;
      xpos+= tft.drawChar(':',xpos,ypos,7);
      if (mm<10) xpos+= tft.drawChar('0',xpos,ypos,7);
      tft.drawNumber(mm,xpos,ypos,7);

// second clock
      tft.setTextColor(ST7735_YELLOW, ST7735_BLACK); // Red
      if (hh2<10) xpos2+= tft.drawChar('0',xpos2,ypos2,7);
      xpos2+= tft.drawNumber(hh2,xpos2,ypos2,7);
      xcolon2=xpos2;
      xpos2+= tft.drawChar(':',xpos2,ypos2,7);
      if (mm<10) xpos2+= tft.drawChar('0',xpos2,ypos2,7);
      tft.drawNumber(mm,xpos2,ypos2,7);   
    }

//animated points for clocks
    if (ss%2) { // Flash the colon
      tft.setTextColor(0x39C4, ST7735_BLACK);
      xpos2+= tft.drawChar(':',xcolon2,ypos2,7);
      //tft.setTextColor(0x39C4, ST7735_BLACK);
      xpos+= tft.drawChar(':',xcolon,ypos,7);

     // tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);
    }
    else {
      tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);
      tft.drawChar(':',xcolon2,ypos2,7);
      tft.setTextColor(ST7735_RED, ST7735_BLACK);
      tft.drawChar(':',xcolon,ypos,7);
      
       }
    
  }
}  // end loop for nivel = 0 (just clock)

if (nivel == 1)   // change hours
{
   
if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  delay(500);
  mm1 = mm;
 sterg();
  }    

   // Update digital time
    byte xpos = 6;
    byte ypos = 0+jos;
   
      tft.setTextColor(ST7735_BLACK, ST7735_BLACK); // Set font colour to back to wipe image
 //     tft.drawString("88:88",xpos,ypos,7); // Overwrite the text to clear it
      tft.drawString("88",xpos,ypos,7); // Overwrite the text to clear it
      tft.setTextColor(ST7735_RED, ST7735_BLACK); // Red
 delay(50);
      
      if (hh1<10) xpos+= tft.drawChar('0',xpos,ypos,7);
      xpos+= tft.drawNumber(hh1,xpos,ypos,7);

       tft.setTextColor(ST7735_RED, ST7735_BLACK); // Red
       
      xcolon=xpos;
      xpos+= tft.drawChar(':',xpos,ypos,7);
      if (mm<10) xpos+= tft.drawChar('0',xpos,ypos,7);
      tft.drawNumber(mm,xpos,ypos,7);
delay(50);
     
if (digitalRead(plus) == LOW)
    {
    hh1 = hh1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)
    {
    hh1 = hh1-1;
    delay(150);
    }    
if (hh1 > 23) hh1 = 0;
if (hh1 < 0) hh1 = 23;       
     
}  // end loop stare = 1 (change the hours)

if (nivel == 2)   // change minutes
{
if (digitalRead(meniu) == LOW)
  {
  delay(500);
  zz1 = zz;  // next step
  sterg();
  nivel = nivel+1;
  }    

   // Update digital time
    byte xpos = 6;
    byte ypos = 0+jos;

  //     tft.drawString("88:88",xpos,ypos,7); // Overwrite the text to clear it

      tft.setTextColor(ST7735_RED, ST7735_BLACK); // Red
      if (hh1<10) xpos+= tft.drawChar('0',xpos,ypos,7);
      xpos+= tft.drawNumber(hh1,xpos,ypos,7);
      xcolon=xpos;
      xpos+= tft.drawChar(':',xpos,ypos,7);
      
     tft.setTextColor(ST7735_BLACK, ST7735_BLACK); // Set font colour to back to wipe image
     tft.drawString("88",xpos,ypos,7); // Overwrite the text to clear it
 delay(50);
       tft.setTextColor(ST7735_RED, ST7735_BLACK); // Red
         
      if (mm1<10) xpos+= tft.drawChar('0',xpos,ypos,7);
      tft.drawNumber(mm1,xpos,ypos,7);
delay(50);
     
if (digitalRead(plus) == LOW)
    {
    mm1 = mm1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)
    {
    mm1 = mm1-1;
    delay(150);
    }    
if (mm1 > 59) mm1 = 0;
if (mm1 < 0) mm1 = 59;       
     
}  // end loop nivel = 2 (change the minutes)

if (nivel == 3)   // change day in week
{
if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  delay(500);
  yy1 = yy;;  // next step
 sterg();
  }    

  for (int y=52+jos; y < 60+jos; y+=1) 
   {
   tft.drawFastHLine(0, y, tft.width(), ST7735_BLACK);
   }
   delay(50);
      tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
      tft.setCursor (15, 52+jos);
      tft.print(daysOfTheWeek[zz1]);
    delay(50);   
     
if (digitalRead(plus) == LOW)
    {
    zz1 = zz1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)
    {
    zz1 = zz1-1;
//    if (zz1 < 0) zz1 = 6;       
    delay(150);
    }    
if (zz1 > 6) zz1 = 0;
if (zz1 < 0) zz1 = 6;       
Serial.println(zz1);
     
}  // end loop stare = 3 (change the day of the week)


if (nivel == 4)   // change year
{
if (digitalRead(meniu) == LOW)
  {
  delay(500);
  ll1 = ll;;  // next step
  sterg();
  nivel = nivel+1;
  }    

  for (int y=52+jos; y < 60+jos; y+=1) 
   {
     tft.drawFastHLine(125, y, 35, ST7735_BLACK);
   }
   delay(50);
      tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
      tft.setCursor (90, 52+jos);
/*      
      if (ll <10) tft.print("0");
      tft.print(ll);
      tft.print("/");
      if (dd <10) tft.print("0");
      tft.print(dd);
      tft.print("/");
*/
      if (dd <10) tft.print("0");
      tft.print(dd);
      tft.print(".");
      if (ll <10) tft.print("0");
      tft.print(ll);
      tft.print(".");
      tft.print(yy1+2000);
    delay(50);   
     
if (digitalRead(plus) == LOW)
    {
    yy1 = yy1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)
    {
    yy1 = yy1-1;
    delay(150);
    }    
if (yy1 > 49) yy1 = 49;
if (yy1 < 16) yy1 = 16;       
     
}  // end loop stare = 4 (change the year)



if (nivel == 5)   // change mounth
{
if (digitalRead(meniu) == LOW)
  {
  delay(500);
  dd1 = dd;;  // next step
  sterg();
  nivel = nivel+1;
  }    

  for (int y=52+jos; y < 60+jos; y+=1) 
   {
//   tft.drawFastHLine(90, y, 14, ST7735_BLACK);
tft.drawFastHLine(106, y, 14, ST7735_BLACK);
   }
   delay(50);
 
      tft.setCursor (90, 52+jos);
      if (dd <10) tft.print("0");
      tft.print(dd);
      tft.print(".");
      if (ll1 <10) tft.print("0");
      tft.print(ll1);
      tft.print(".");
      tft.print(yy1+2000);    
    delay(50);   
     
if (digitalRead(plus) == LOW)
    {
    ll1 = ll1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)
    {
    ll1 = ll1-1;
    delay(150);
    }    
   
if (ll1 > 12) ll1 = 1;
if (ll1 < 1) ll1 = 12;       
     
}  // end loop stare = 5 (change the day as data)



if (nivel == 6)   // change day as data
{
if (digitalRead(meniu) == LOW)
  {
   delay(500);
   sterg();
   nivel = nivel+1;
  }    

  for (int y=52+jos; y < 60+jos; y+=1) 
   {
tft.drawFastHLine(90, y, 14, ST7735_BLACK);    
//   tft.drawFastHLine(106, y, 14, ST7735_BLACK);
   }
   delay(50);
 
      tft.setCursor (90, 52+jos);
  /*
      if (ll1 <10) tft.print("0");
      tft.print(ll1);
      tft.print("/");
      if (dd1 <10) tft.print("0");
      tft.print(dd1);
      tft.print("/");
  */
    if (dd1 <10) tft.print("0");
      tft.print(dd1);
      tft.print(".");
      if (ll1 <10) tft.print("0");
      tft.print(ll1);
      tft.print(".");    
      tft.print(yy1+2000);
      delay(50);   
     
if (digitalRead(plus) == LOW)
    {
    dd1 = dd1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)
    {
    dd1 = dd1-1;
    delay(150);
    }    
    
 // Dawn & Dusk controller. http://andydoz.blogspot.ro/2014_08_01_archive.html
 if (ll == 4 || ll == 5 || ll == 9 || ll == 11) { //30 days hath September, April June and November
    maxday = 30;
  }
  else {
  maxday = 31; //... all the others have 31
  }
  if (ll ==2 && yy % 4 ==0) { //... Except February alone, and that has 28 days clear, and 29 in a leap year.
    maxday = 29;
  }
  if (ll ==2 && ll % 4 !=0) {
    maxday = 28;
  }

if (dd1 > maxday) dd1 = 1;
if (dd1 < 1) dd1 = maxday;       
     
}  // end loop nivel = 6 (change the day as data)


if (nivel == 7)   // change hours
{
   
if (digitalRead(meniu) == LOW)
  {
  delay(500);
  sterg();
  nivel = nivel+1;
  }    

      tft.setCursor (90, 42+jos);
      tft.print("GMT");
      
  for (int y=52+jos; y < 60+jos; y+=1) 
   {
tft.drawFastHLine(90, y, 69, ST7735_BLACK);
   }
   delay(50);
 
      tft.setCursor (90, 52+jos);
      if (hh3 > 0) tft.print("+");
      tft.print(hh3);
      delay(50);   

     
if (digitalRead(plus) == LOW)
    {
    hh3 = hh3+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)
    {
    hh3 = hh3-1;
    delay(150);
    }    
if (hh3 > 12) hh3 = 12;
if (hh3 < -12) hh3 = -12;       
     
}  // end loop stare = 7 (change the hours)



 if (nivel == 8)  // adjust the clock
 {
sterg();

// setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year);
setDS3231time(0, mm1, hh1, zz1, dd1, ll1, yy1);

if (hh3 > 0)
{
  hh4 = hh3;
  semn = 1;
}
if (hh3 <0)
{
  hh4 = -hh3;
  semn = 0;
}
if (hh3 == 0)
{
  hh4 = 0;
  semn = 1;
}

  EEPROM.write(100, hh4);
  EEPROM.write(101, semn);
  

 nivel = 9;
 } // end loop nivel = 8

 
 if (nivel >=9)  // readfy to return to main loop
 {nivel = 0;  
 omm = 99;
 temperatura0 = 0.0;
 initial = 1;
 }
}  // end main loop

// http://forum.arduino.cc/index.php?topic=398891.0
void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
                   dayOfMonth, byte month, byte year)
{
    // sets time and date data to DS3231
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0); // set next input to start at the seconds register
    Wire.write(decToBcd(second)); // set seconds
    Wire.write(decToBcd(minute)); // set minutes
    Wire.write(decToBcd(hour)); // set hours
    Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
    Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
    Wire.write(decToBcd(month)); // set month
    Wire.write(decToBcd(year)); // set year (0 to 99)
    Wire.endTransmission();
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
    return ( (val / 16 * 10) + (val % 16) );
}

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
    return ( (val / 10 * 16) + (val % 10) );
}

void readDS3231time(byte *second,
                    byte *minute,
                    byte *hour,
                    byte *dayOfWeek,
                    byte *dayOfMonth,
                    byte *month,
                    byte *year)
{
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0); // set DS3231 register pointer to 00h
    Wire.endTransmission();
    Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
    // request seven bytes of data from DS3231 starting from register 00h
    *second = bcdToDec(Wire.read() & 0x7f);
    *minute = bcdToDec(Wire.read());
    *hour = bcdToDec(Wire.read() & 0x3f);
    *dayOfWeek = bcdToDec(Wire.read());
    *dayOfMonth = bcdToDec(Wire.read());
    *month = bcdToDec(Wire.read());
    *year = bcdToDec(Wire.read());
}

void sterg()
   {for (int y=0; y < 127; y+=1) 
   {
   tft.drawFastHLine(0, y, tft.width(), ST7735_BLACK);
   }
   }
