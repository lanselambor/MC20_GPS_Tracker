#include "MC20_Common.h"
#include "MC20_Arduino_Interface.h"
#include "MC20_GNSS.h"


GNSS gnss = GNSS();

void setup() {
  SerialUSB.begin(115200);
  // while(!SerialUSB);

  gnss.Power_On();
  SerialUSB.println("\n\rPower On!");

  while(!gnss.open_GNSS(EPO_RL_MODE)){
    delay(1000);
  }

  SerialUSB.println("Open GNSS OK.");
}

void loop() {
  // gnss.dataFlowMode();
  
  if(gnss.getCoordinate()){
    SerialUSB.print("GNSS: ");
    SerialUSB.print(gnss.longitude, 6); 
    SerialUSB.print(",");
    SerialUSB.println(gnss.latitude, 6);  
  } else{
    SerialUSB.println("Error!");
  }
  
  delay(1000);
}
