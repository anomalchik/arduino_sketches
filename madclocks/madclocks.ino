#include "RTClib.h"

#include <GxEPD2_BW.h>
#include "Fonts/madclocks12pt7b.h"
#include "Fonts/FreeMonoBold9pt7b.h"

#define ENABLE_GxEPD2_GFX 0
#define MAX_DISPAY_BUFFER_SIZE 100 // 800
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPAY_BUFFER_SIZE / (EPD::WIDTH / 8))
GxEPD2_BW<GxEPD2_154, MAX_HEIGHT(GxEPD2_154)> display(GxEPD2_154(/*CS=10*/ SS, /*DC=*/ 8, /*RST=*/ -1, /*BUSY=*/ 7));

char* minDigits[] = {"JLYF", "LDT", "NHB", "XTNSHT", "GZNM", "ITCNM", "CTVM", "DJCTVM", "LTDZNM", 
                    "LTCZNM", "JLBYYFLWFNM", "LDTYFLWFNM", "NHBYFLWFNM", "XTNSHYFLWFNM", "GZNYFLWFNM", "ITCNYFLWFNM", "CTVYFLWFNM", "DJCTVYFLWFNM", "LTDZNYFLWFNM"};

char* decMinDigits[] = {"LDFLWFNM", "NHBLWFNM", "CJHJR", "GZNMLTCZN"}; //20, 30, 40, 50 min

char* hours[] = {"GTHDJUJ", "DNJHJUJ", "NHTNMTUJ", "XTNDTHNJUJ",
                "GZNJUJ", "ITCNJUJ", "CTLMVJUJ", "DJCMVJUJ",
                "LTDZNJUJ", "LTCZNJUJ", "JLBYYFLWFNJUJ", "LTDFYLWFNJUJ"};

int currTime[3];

int oldMin = -1;
int newMin = 0; //check min state for display refresh

RTC_DS3231 rtc;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  display.init(115200);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }


  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop()
{
  DateTime now = rtc.now();
  getData(now);
  rtclibTest(now);
  if (newMin != oldMin) {
    showData();
    showTemperature();
    display.powerOff();
    oldMin = newMin;
  }
  delay(3000);
}


void rtclibTest (DateTime now)
{
    Serial.print('/');
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(now.dayOfTheWeek());
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");
}

void getData (DateTime now)
{
  currTime[0] = now.hour();
  currTime[1] = now.minute()/10;
  currTime[2] = now.minute()%10;
  newMin=currTime[2];
}

void showData ()
{
  int16_t start_posy = 33;
  display.setFullWindow();
  display.setRotation(2);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&madclocks12pt7b);
  if (currTime[0] > 12)
    currTime[0]=currTime[0]-12;
  display.firstPage();
    do
    {
      display.fillScreen(GxEPD_WHITE);
      if (currTime[1] > 1 && currTime[2] != 0) {
        display.setCursor(0, start_posy);
        display.print(decMinDigits[currTime[1]-2]);
        display.setCursor(0, start_posy+30); //20+30
        display.print(minDigits[currTime[2]-1]);
      }else if (currTime[1] > 1 && currTime[2] == 0){
        display.setCursor(0, start_posy+30);
        display.print(decMinDigits[currTime[1]-2]);  
      }else if (currTime[1] == 1 && currTime[2] >= 0){
        display.setCursor(0, start_posy+30);
        display.print(minDigits[9+currTime[2]]);       
      }else if (currTime[1] == 0 && currTime[2] == 0){
        display.setCursor(0, start_posy);
        display.print("HJDYJ");
      }else if (currTime[2] == 0) {
        display.setCursor(0, start_posy);
        display.print(currTime[1]); 
      }else{
        display.setCursor(0, start_posy+30); //20+30
        display.print(minDigits[currTime[2]-1]);
      }
      display.setCursor(0, start_posy+120); //20+30+30+30+30
      display.print(hours[currTime[0]]);
    
      display.setCursor(80, start_posy+60); //20+30+30
      display.print("uht,fys[");
      display.setCursor(112, start_posy+90); //20+30+30+30
      display.print("vbyen");
      display.setCursor(65, start_posy+150); //20+30+30+30+30+30
      display.print("vfnm dfie");

      
    }
    while (display.nextPage());
}
void showTemperature ()
{
  int16_t start_posy = 33;
  display.setPartialWindow(5, 160, 50, 200);
  display.setRotation(2);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.firstPage();
    do
    {
      //Temperature
      display.setFont(&FreeMonoBold9pt7b);
      display.setCursor(5, start_posy+150);
      display.print(int(rtc.getTemperature()));
      display.setCursor(29, start_posy+150);
      display.print("C");
    }
    while (display.nextPage());
}
