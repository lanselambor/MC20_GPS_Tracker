#include "MC20_Common.h"
#include "MC20_Arduino_Interface.h"

#define RGBPIN 10

const char message[128] = "Hello MC20!";

GPSTracker gpsTracker = GPSTracker();

void setup() {
  pinMode(RGBPIN, OUTPUT);
  digitalWrite(RGBPIN, LOW);
  SerialUSB.begin(115200);
  while(!SerialUSB);

  gpsTracker.Power_On();
  SerialUSB.println("Power On!");

<<<<<<< HEAD
  if(!gpsTracker.waitForNetworkRegister())
  {
    SerialUSB.println("Network error!");
    return;
  }
  
  bool ret = gpsTracker.sendSMS("13750024343", "Hello MC20!");

  SerialUSB.print("ret: ");
  SerialUSB.println(ret);
=======
  gpsTracker.sendSMS("xxxxxxxxxxx", "Hello MC20!");
>>>>>>> bc4c74bd16f833fc95fb9237d0c55b55ad65699c
}

void loop() {
  /* Debug */
  if(SerialUSB.available()){
    serialMC20.write(SerialUSB.read());
  }
  if(serialMC20.available()){     
    SerialUSB.write(serialMC20.read()); 
  }

}