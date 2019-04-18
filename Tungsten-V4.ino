/********************************************************************************************************
* Arduino Control Brushless ESC
*
* Description
* program for controlling Brushless ESC for Nerf flywheel blasters
*
* modified 19 Feb 2019
* by TungstenEXE
*
* If you find my code useful, do support me by subscribing my YouTube Channel, thanks.
*
* My YouTube Channel Link - Nerf related
* https://www.youtube.com/Tungstenexe
* 
* ESC used     - Hobbywing BLHeli_32_Dshot1200 XRotor-30A-Micro-2-5S-
********************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include the Bounce2 library found here :
// https://github.com/thomasfredericks/Bounce-Arduino-Wiring
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Bounce2.h>
#include <Servo.h>

#define PIN_FW_CW__ESC      3     // PIN to control ESC, normally the white wire from ESC 
#define PIN_FW_CCW_ESC      11    // PIN to control ESC, normally the white wire from ESC 
#define PIN_BTN             7     // PIN for button to Rev

#define PIN_PMETER          A0    // PIN Potentiometer

/*///////////////////////////////////////////////////////////////////////////////////////////////////////
 * The following are the setting for the ESC used, for other ESC, just chnage setting according to
 * ESC Spec
 *///////////////////////////////////////////////////////////////////////////////////////////////////////
#define THROTTLE_MIN        1500
#define THROTTLE_MAX        2000
#define THROTTLE_BAKE       1000
// End of ESC setting ///////////////////////////////////////////////////////////////////////////////////

int     throttle        = THROTTLE_BAKE;                        // default throttle
int     throttleUpInc   = (THROTTLE_MAX - THROTTLE_BAKE) / 25;  // the increament
int     throttleDownInc = (THROTTLE_MAX - THROTTLE_BAKE) / 500; // the increament
int     throttleSetted  = THROTTLE_BAKE;

unsigned long delayIntervaltime;
unsigned long delayIntervalRevUp   = 8;   // in millis
unsigned long delayIntervalRevDown = 12;  // in millis

Servo   fw_cw__ESC;                                          // to control ESC
Servo   fw_ccw_ESC;                                          // to control ESC

Bounce  btnRev          = Bounce();                          // to track rev button

boolean isRevUp         = false;
boolean isRevDown       = false;
boolean potentiometerOn = false;

void setup() {
  
  pinMode(PIN_BTN, INPUT_PULLUP);   // Using Internal PULLUP resistor
  btnRev.attach(PIN_BTN);
  btnRev.interval(5);
  
  fw_cw__ESC.attach(PIN_FW_CW__ESC);
  fw_ccw_ESC.attach(PIN_FW_CCW_ESC);
 
  fw_cw__ESC.writeMicroseconds(THROTTLE_BAKE);
  fw_ccw_ESC.writeMicroseconds(THROTTLE_BAKE);
  delay(4000);
}

void loop() {
  btnRev.update();                  // update the button status

  if (btnRev.fell()) {              // button pressed    
    throttleSetted = (potentiometerOn) ? map(analogRead(PIN_PMETER), 0, 1023, THROTTLE_MIN, THROTTLE_MAX) : THROTTLE_MAX;    
    isRevUp   = true;
    isRevDown = false;
    delayIntervaltime = millis();         
  } else if (btnRev.rose()) {       // button released
    throttleSetted = THROTTLE_BAKE;
    isRevUp   = false;
    isRevDown = true; 
    delayIntervaltime = millis(); 
  }

  if (isRevUp) {
    handleRevUp();
  } else if (isRevDown) {
    handleRevDown();
  } 
  
}

void handleRevUp() {
  unsigned long durPassed = millis() - delayIntervaltime;

  if (durPassed >= delayIntervalRevUp) {
    throttle += throttleUpInc;
    if (throttle >= throttleSetted) {
      throttle = throttleSetted;
      isRevUp = false;
    }
    fw_cw__ESC.writeMicroseconds(throttle);
    fw_ccw_ESC.writeMicroseconds(throttle);
    delayIntervaltime = millis();
  }
}

void handleRevDown() {
  unsigned long durPassed = millis() - delayIntervaltime;

  if (durPassed >= delayIntervalRevDown) {
    throttle -= throttleDownInc;
    if (throttle <= throttleSetted) {
      throttle = throttleSetted;
      isRevDown = false;
    }
    fw_cw__ESC.writeMicroseconds(throttle);
    fw_ccw_ESC.writeMicroseconds(throttle);
    delayIntervaltime = millis();
  }
}
