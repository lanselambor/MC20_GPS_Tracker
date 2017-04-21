#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SPI.h>
#include <SD.h>
#include "MC20_GNSS.h"
#include "MC20_Common.h"

File myFile;
char* gpsFileName = "gps.txt";

GNSS gnss = GNSS();

void setup() {
  SerialUSB.begin(115200);
  while(!SerialUSB);
  gnss.Power_On();
  SerialUSB.println("Power On!");

  while(!gnss.open_GNSS()){
    delay(1000);
  }
  SerialUSB.println("Open GNSS OK.");

  // SD card Initialize!
  SerialUSB.println("Initializing SD card...");

  if (!SD.begin(4)) {
    SerialUSB.println("initialization failed!");
    return;
  }
  // createNewFile("gps.txt");
  if(SD.exists(gpsFileName)){
    SD.remove(gpsFileName);
  }

  SerialUSB.println("SD card initialization done.");

}


void loop() {
  SerialUSB.println("Write file...");
  gnss.getCoordinate();
  myFile = SD.open(gpsFileName, FILE_WRITE);

  if(myFile){
    myFile.print(gnss.longitude);
    myFile.print(" ");
    myFile.println(gnss.latitude);
    myFile.println();
  }

  myFile.close();

  delay(2000);

}