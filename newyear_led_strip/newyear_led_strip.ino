#include <EEPROM.h>
#include <GyverButton.h>

#define BUTTON 7
#define CHANNEL1 6
#define CHANNEL2 5

/* EEPROM BLOCKS START */
#define DELAY_BLOCK 0
#define BMODE_BLOCK 2

/* Delays (ms) */
int maxDelay=300;
int delayStep=25;
int pModeBlinkCycles=16;

/* Pin voltage */
int pinHigh=40; // 0.78V
int pinLow=0;
int channelState[2];
int blinkCycle;

/* eeprom flasgs */
int currentDelay;
int blinkMode;

unsigned long prevMillis=0;

GButton btn(BUTTON, HIGH_PULL, NORM_OPEN);

void setup() {
  /* Pin Section */
  pinMode(BUTTON, INPUT_PULLUP); //with built-in resistor
  pinMode(CHANNEL1, OUTPUT);
  pinMode(CHANNEL2, OUTPUT);

  Serial.begin(9600);
  
  /* Validate data in EEPROM blocks*/
  getEEPROM();
  if (currentDelay <= 49 || currentDelay > maxDelay ){
    EEPROM.put(DELAY_BLOCK, maxDelay);
    Serial.println("currentDelay eeprom block cleaned!");
  }
  if (blinkMode > 2 || blinkMode < 0 ){
    EEPROM.put(BMODE_BLOCK, 0);
    Serial.println("blinkMode eeprom block cleaned!");
  }
  
  getEEPROM();
  Serial.println(currentDelay);
  Serial.println(blinkMode);

  /* set button timeouts */
  buttonSetup();
  returnStockState();
}

void loop() {
  btn.tick(); //button listener

  if (btn.isClick()){
    currentDelay-=delayStep;
    
    if (currentDelay < 50) {
      currentDelay=maxDelay;
    }
      EEPROM.put(DELAY_BLOCK, currentDelay);
      Serial.print("currentDelay is ");
      Serial.println(currentDelay);
  }
  if (btn.isHolded()) {
    blinkMode++;
    if (blinkMode>2) blinkMode=0;
    returnStockState();
    EEPROM.put(BMODE_BLOCK, blinkMode);
    Serial.print("blinkMode is ");
    Serial.println(blinkMode);
  }
  
  if (blinkMode==1){
    if (blinkCycle<pModeBlinkCycles/2){
      if (channelState[0]==pinHigh && ((pModeBlinkCycles/2)%2==0) ){
        blinkLed(CHANNEL1,pinLow,0.5);
      }else if(channelState[0]==pinLow){
        blinkLed(CHANNEL1,pinHigh,0.5);
      }
    }else{
      if (channelState[1]==pinHigh && ((pModeBlinkCycles/2)%2==0) ){
        blinkLed(CHANNEL2,pinLow,0.5);
      }else if(channelState[1]==pinLow){
        blinkLed(CHANNEL2,pinHigh,0.5);
      }
    }
  }else if(blinkMode==2){
    if (channelState[0]==pinHigh && blinkCycle==1){
      if (blinkLed(CHANNEL1,pinLow,1)){
        analogWrite(CHANNEL2, pinHigh);
        logChannel(CHANNEL2, pinHigh, true);
      }
    }else if(channelState[0]==pinLow && blinkCycle==0){
      if (blinkLed(CHANNEL1,pinHigh,1)){
        analogWrite(CHANNEL2, pinLow);
        logChannel(CHANNEL2, pinLow, true);
      }
    } 
  }else{
    if (channelState[0]==pinHigh && blinkCycle==3){
      blinkLed(CHANNEL1,pinLow,1);
    }else if(channelState[0]==pinLow && blinkCycle==0){
      blinkLed(CHANNEL1,pinHigh,1);
    }else if(channelState[1]==pinHigh && blinkCycle==1){
      blinkLed(CHANNEL2,pinLow,1);
    }else if(channelState[1]==pinLow && blinkCycle==2){
      blinkLed(CHANNEL2,pinHigh,1);
    } 
  }

}

/* Read data in EEPROM blocks*/
void getEEPROM()
{
  EEPROM.get(DELAY_BLOCK, currentDelay);
  EEPROM.get(BMODE_BLOCK, blinkMode);
}

void buttonSetup()
{
  btn.setDebounce(50);
  btn.setTimeout(500);
  btn.setClickTimeout(600);
}

int blinkLed(int channel, int pinState, float k)
{
  int chNum;
  if (channel == CHANNEL1) {
    chNum=0;
  }else if(channel == CHANNEL2) {
    chNum=1;
  }

  unsigned long currentMillis = millis();
  if (currentMillis - prevMillis >= currentDelay*k) {
    prevMillis=currentMillis;
    analogWrite(channel, pinState);
    channelState[chNum]=pinState;
    blinkCycle++;
    if ((blinkMode==0 && blinkCycle>3) || (blinkMode==1 && blinkCycle>(pModeBlinkCycles-1)) || (blinkMode==2 && blinkCycle>1))
      returnStockState();
    logChannel(channel, pinState, false);
    return 1;
  }
  return 0;
}

void returnStockState()
{
  if (!blinkMode)
  {
    channelState[0]=pinLow;
    channelState[1]=pinHigh;
  }else{
    channelState[0]=pinLow;
    channelState[1]=pinLow;
  }
  blinkCycle=0;
}

void logChannel(int channel, int pinState, boolean spacer)
{
  Serial.print("Channel: ");
  Serial.print(channel);
  Serial.print(" ");
  Serial.print("State: ");
  Serial.println(pinState);
  if (spacer) Serial.println(" "); 
}
