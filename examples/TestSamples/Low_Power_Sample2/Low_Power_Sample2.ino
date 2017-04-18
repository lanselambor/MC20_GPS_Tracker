#include "MC20_Common.h"
#include "MC20_Arduino_Interface.h"
#include "MC20_GNSS.h"

#define RGBPIN 10

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
}

void loop() {
  gnss.dataFlowMode();
  gnss.close_GNSS();
  delay(5000);
  gnss.open_GNSS();
  delay(5000);
}