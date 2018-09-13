/*********************************
 * Sketch version: 1.1           *
 *********************************
 * This sketch just experiment!! * 
 * RC for overhead crane         *
 * by Anomalchik                 *
 *********************************/
 
/*
 * Changelog
 * 1.1 Added blocking for paired buttons
 */

// NOTE: Need RemoteXY library v 2.3.4
// NOTE2: in RemoteXY_API.h reduced REMOTEXY_TIMEOUT to 1000.

///////////////////////////////////////////// 
//        RemoteXY include library         // 
///////////////////////////////////////////// 

/* RemoteXY select connection mode and include library */ 
#define REMOTEXY_MODE__ESP8266_HARDSERIAL_POINT 
#include <RemoteXY.h> 

/* RemoteXY connection settings */ 
#define REMOTEXY_SERIAL Serial 
#define REMOTEXY_SERIAL_SPEED 115200 
#define REMOTEXY_WIFI_SSID "OverheadRXY" 
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377

/* RemoteXY configurate  */ 
#pragma pack(push, 1) 
uint8_t RemoteXY_CONF[] = 
  { 255,6,0,0,0,69,0,8,0,1,
  1,0,26,5,12,12,2,31,77,86,
  0,1,0,26,20,12,12,2,31,77,
  78,0,1,0,26,35,12,12,2,31,
  84,86,0,1,0,26,50,12,12,2,
  31,84,78,0,1,0,26,65,12,12,
  2,31,86,82,0,1,0,26,80,12,
  12,2,31,77,73,0 };


/* this structure defines all the variables of your control interface */ 
struct { 

    /* input variable */
  uint8_t CFORWARD; // =1 if button pressed, else =0 
  uint8_t CBACKWARD; // =1 if button pressed, else =0

  uint8_t TFORWARD; // =1 if button pressed, else =0
  uint8_t TBACKWARD; // =1 if button pressed, else =0

  uint8_t VIRA; // =1 if button pressed, else =0
  uint8_t MAINA; // =1 if button pressed, else =0  

    /* other variable */
  unsigned char connect_flag;  /* =1 if RemoteXY is connected, else =0 */

} RemoteXY; 
#pragma pack(pop) 

///////////////////////////////////////////// 
//           END RemoteXY include          // 
///////////////////////////////////////////// 

#define RELAY_DEFAULT_STATE 1 //1 - HIGH, 0 - LOW
#define NUMBER_OF_RELAYS 7 // 7 active relays

int relays[NUMBER_OF_RELAYS]={2,3,4,5,6,7,8}; //relay pins
/*  relay[0] - crane forward
 *  relay[1] - crane backward
 *  relay[2] - telpher forward
 *  relay[3] - telpher backward
 *  relay[4] - lift up
 *  relay[5] - let down
 *  relay[6] - main phase control circuit
 */

void setup() {
  RemoteXY_Init (); 
  pinMode(13, OUTPUT);
  digitalWrite(13, 0); //disable led pin
  int i;
  for (i=0; i<=NUMBER_OF_RELAYS-1; i++) { //setup all relays pins as output and disable it!
    pinMode(relays[i], OUTPUT);
    digitalWrite(relays[i], RELAY_DEFAULT_STATE);
  }
}

void disableAllButtons() {
  RemoteXY.CFORWARD=0;
  RemoteXY.CBACKWARD=0;
  RemoteXY.TFORWARD=0;
  RemoteXY.TBACKWARD=0;
  RemoteXY.VIRA=0;
  RemoteXY.MAINA=0;
}

void disableAllRelays() {
  int i;
  for (i=0; i<=NUMBER_OF_RELAYS-1; i++) {
    digitalWrite(relays[i], RELAY_DEFAULT_STATE);
  }
}

void loop() {
  RemoteXY_Handler (); 
  if (RemoteXY.connect_flag > 0) {
    digitalWrite(relays[6], !RELAY_DEFAULT_STATE); // Enable control circuit if RemoteXY connected
    // crane
    if (RemoteXY.CBACKWARD == 0) {
      digitalWrite(relays[0], (RemoteXY.CFORWARD==0)?RELAY_DEFAULT_STATE:!RELAY_DEFAULT_STATE);
    }
    if (RemoteXY.CFORWARD == 0) {
      digitalWrite(relays[1], (RemoteXY.CBACKWARD==0)?RELAY_DEFAULT_STATE:!RELAY_DEFAULT_STATE);
    }
    // telpher
    if (RemoteXY.TBACKWARD == 0) {
      digitalWrite(relays[2], (RemoteXY.TFORWARD==0)?RELAY_DEFAULT_STATE:!RELAY_DEFAULT_STATE);
    }
    if (RemoteXY.TFORWARD == 0) {
      digitalWrite(relays[3], (RemoteXY.TBACKWARD==0)?RELAY_DEFAULT_STATE:!RELAY_DEFAULT_STATE);
    }
    // lift
    if (RemoteXY.MAINA == 0) {
      digitalWrite(relays[4], (RemoteXY.VIRA==0)?RELAY_DEFAULT_STATE:!RELAY_DEFAULT_STATE);
    }
    if (RemoteXY.VIRA == 0) {
      digitalWrite(relays[5], (RemoteXY.MAINA==0)?RELAY_DEFAULT_STATE:!RELAY_DEFAULT_STATE);
    }
  }else{
    disableAllButtons(); //disable all buttons if connection is lost
    disableAllRelays(); //disable all relays if connection is lost
  }
}
