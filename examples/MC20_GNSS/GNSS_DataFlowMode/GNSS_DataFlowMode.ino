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

  while(!gnss.open_GNSS()){
    delay(1000);
  }

  SerialUSB.println("Open GNSS OK.");
}

void loop() {
  gnss.dataFlowMode();
  
  // if(gnss.getCoordinate()){
  //   SerialUSB.print("GNSS: ");
  //   SerialUSB.print(gnss.longitude); 
  //   SerialUSB.print(",");
  //   SerialUSB.println(gnss.latitude);  
  // } else{
  //   SerialUSB.println("Error!");
  // }
  
  delay(1000);
}
