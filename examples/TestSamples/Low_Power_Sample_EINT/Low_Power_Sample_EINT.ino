#include "MC20_Common.h"
#include "MC20_Arduino_Interface.h"
#include "MC20_GNSS.h"
#include <EnergySaving.h>

#define RGBPIN 10

EnergySaving nrgSave;

GNSS gnss = GNSS();

void setup() {
  nrgSave.begin(WAKE_EXT_INTERRUPT, 3, dummy);  //standby setup for external interrupts
}

void initialize()
{
  pinMode(RGBPIN, OUTPUT);
  digitalWrite(RGBPIN, LOW);
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);

  SerialUSB.begin(115200);
  // while(!SerialUSB);

  gnss.Power_On();
  SerialUSB.println("\n\rPower On!");

  if(!gnss.open_GNSS(GNSS_DEFAULT_MODE)){
    return;
  }

  SerialUSB.println("Open GNSS OK.");
}


void loop() {
  initialize();
  delay(5000);
  for(int i = 0;i < 10; i++){
    gnss.dataFlowMode();
    delay(1000); 
  }
  gnss.close_GNSS();
  Serial1.end();
  SerialUSB.end();
  nrgSave.standby();  //now mcu goes in standby mode
}

void dummy(void)  //interrupt routine (isn't necessary to execute any tasks in this routine
{
  
}