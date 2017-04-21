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

  if(!gnss.open_GNSS(GNSS_DEFAULT_MODE)){
    return;
  }

  //gnss.enable_GLP(1,0);

  SerialUSB.println("Open GNSS OK.");
  delay(2000);

  //gnss.set1PPS(true);
  
  // for(int i = 0; i < 5; i++){
  //   gnss.dataFlowMode();
  //   delay(1000);  
  // }

  //gnss.queryData_LOCUS();
}

void loop() {
  /* Debug */
  // if(SerialUSB.available()){
  //   serialMC20.write(SerialUSB.read());
  // }
  // if(serialMC20.available()){     
  //   SerialUSB.write(serialMC20.read()); 
  // }

  gnss.dataFlowMode();
  delay(1000);  
}
