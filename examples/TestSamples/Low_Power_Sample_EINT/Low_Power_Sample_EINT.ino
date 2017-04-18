#include "MC20_Common.h"
#include "MC20_Arduino_Interface.h"
#include "MC20_GNSS.h"
#include <EnergySaving.h>

#define RGBPIN 10

EnergySaving nrgSave;

GNSS gnss = GNSS();

void setup() {
  pinMode(RGBPIN, OUTPUT);
  digitalWrite(RGBPIN, LOW);
  SerialUSB.begin(115200);
  // while(!SerialUSB);

  gnss.Power_On();
  SerialUSB.println("\n\rPower On!");

  if(!gnss.open_GNSS(EPO_RL_MODE)){
    return;
  }

  SerialUSB.println("Open GNSS OK.");
  nrgSave.begin(WAKE_EXT_INTERRUPT, 3, dummy);  //standby setup for external interrupts
}

void loop() {
  gnss.open_GNSS();
  delay(5000);
  gnss.dataFlowMode();
  gnss.close_GNSS();
  Serial1.flush();
  SerialUSB.flush();
  nrgSave.standby();  //now mcu goes in standby mode
}

void dummy(void)  //interrupt routine (isn't necessary to execute any tasks in this routine
{
  
}