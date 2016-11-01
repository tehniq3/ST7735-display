// original sketch from http://educ8s.tv/arduino-real-time-clock/
// Nicu FLORICA (niq_ro) from http://www.arduinotehniq.com 
// made some chenages for animate display with blinking signs


#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include "Sodaq_DS3231.h"
#include <SPI.h>

#define TFT_CS     9
#define TFT_RST    7                      
#define TFT_DC     8
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!

float maxTemperature=0;
float minTemperature=200;
char charMinTemperature[10];
char charMaxTemperature[10];
char timeChar[100];
char dateChar[50];
char temperatureChar[10];

float temperature = 0;
float previousTemperature = 0;

String dateString;
int minuteNow=0;
int minutePrevious=0;

void setup () 
{
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST7735_BLACK);
    Serial.begin(57600);
    Wire.begin();
    rtc.begin();
    printText("TEMPERATURE", ST7735_GREEN,30,65,1);  // Temperature Static Text
    printText("MAX", ST7735_RED,18,130,1);
    printText("MIN", ST7735_BLUE,95,130,1);
 //   setRTCTime();
}

uint32_t old_ts;

void loop () 
{
  float temperature = rtc.getTemperature();
  DateTime now = rtc.now(); //get the current date-time
  uint32_t ts = now.getEpoch();

    if (old_ts == 0 || old_ts != ts) {
	old_ts = ts;
  
  minuteNow = now.minute();
  if(minuteNow!=minutePrevious)
  {
    dateString = getDayOfWeek(now.dayOfWeek())+", ";
    dateString = dateString+String(now.date())+"/"+String(now.month());
    dateString= dateString+"/"+ String(now.year()); 
    minutePrevious = minuteNow;
    String hours = "";
    if(now.hour()<10)
    {
         hours = hours+" "+String(now.hour());
    }else  
    hours = hours+ String(now.hour());
    if(now.minute()<10)
    {
        hours = hours+" 0"+String(now.minute());
    }else
    {
        hours = hours+" "+String(now.minute());
    }
    
    hours.toCharArray(timeChar,100);
    tft.fillRect(10,0,160,65,ST7735_BLACK);
  //  printText(timeChar, ST7735_WHITE,20,25,3);
    printText(timeChar, ST7735_YELLOW,20,25,3);
    dateString.toCharArray(dateChar,50);
    printText(dateChar, ST7735_GREEN,8,5,1);
  }
  
  if(temperature != previousTemperature)
  {
    previousTemperature = temperature;
    String temperatureString = String(temperature,1);
    temperatureString.toCharArray(temperatureChar,10);
    tft.fillRect(10,80,128,30,ST7735_BLACK);
    printText(temperatureChar, ST7735_WHITE,10,80,3);
    printText("o", ST7735_WHITE,90,75,2);
    printText("C", ST7735_WHITE,105,80,3);

    if(temperature>maxTemperature)
    {
      maxTemperature = temperature;
      dtostrf(maxTemperature,5, 1, charMaxTemperature); 
      tft.fillRect(3,142,33,20,ST7735_BLACK);
      printText(charMaxTemperature, ST7735_WHITE,3,145,1);
      printText("o", ST7735_WHITE,35,140,1);
      printText("C", ST7735_WHITE,41,145,1);
    }
    if(temperature<minTemperature)
    {
      minTemperature = temperature;
      dtostrf(minTemperature,5, 1, charMinTemperature); 
      tft.fillRect(75,140,36,18,ST7735_BLACK);
      printText(charMinTemperature, ST7735_WHITE,80,145,1);
      printText("o", ST7735_WHITE,112,140,1);
      printText("C", ST7735_WHITE,118,145,1);
    }
  }
}
delay(500);
//     printText("o", ST7735_WHITE,62,28,1);
//     printText("o", ST7735_WHITE,62,35,1);
     printText("o", ST7735_YELLOW,62,28,1);
     printText("o", ST7735_YELLOW,62,35,1);

delay(500);
     printText("o", ST7735_BLACK,62,28,1);
     printText("o", ST7735_BLACK,62,35,1);
}

void setRTCTime()
{
  //DateTime dt(2015, 8, 27, 9, 35, 30, 4); // Year, Month, Day, Hour, Minutes, Seconds, Day of Week
DateTime dt(2016, 10, 31, 14, 55, 30, 1); // Year, Month, Day, Hour, Minutes, Seconds, Day of Week
  rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above 
}

void printText(char *text, uint16_t color, int x, int y,int textSize)
{
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(textSize);
  tft.setTextWrap(true);
  tft.print(text);
}

String getDayOfWeek(int i)
{
  switch(i)
  {
    case 1: return "Monday";break;
    case 2: return "Tuesday";break;
    case 3: return "Wednesday";break;
    case 4: return "Thursday";break;
    case 5: return "Friday";break;
    case 6: return "Saturday";break;
    case 7: return "Sunday";break;
    default: return "Monday";break;
  }
}
