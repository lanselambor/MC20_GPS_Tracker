#include "MC20_Common.h"
#include "MC20_Arduino_Interface.h"
#include "MC20_GNSS.h"
#include <SeeedOLED.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;
File myFile;
char* gpsFileName = "gps.txt";

GNSS gnss = GNSS();

void setup() {
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);
  SerialUSB.begin(115200);
  // while(!SerialUSB);

  gnss.Power_On();
  SerialUSB.println("\n\rPower On!");

  while(!gnss.open_GNSS()){
    delay(1000);
  }

  SerialUSB.println("Open GNSS OK.");

  Wire.begin();
  SeeedOled.init();  //initialze SEEED OLED display

  SeeedOled.clearDisplay();          //clear the screen and set start position to top left corner
  SeeedOled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();           //Set addressing mode to Page Mode
  
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
  // gnss.dataFlowMode();
  
  // if(gnss.getCoordinate()){
  //   SerialUSB.print("GNSS: ");
  //   SerialUSB.print(gnss.longitude); 
  //   SerialUSB.print(",");
  //   SerialUSB.println(gnss.latitude);  
  // } else{
  //   SerialUSB.println("Error!");
  // }


  gnss.getCoordinate();
  SerialUSB.print("GNSS: ");
  SerialUSB.print(gnss.longitude, 6); 
  SerialUSB.print(",");
  SerialUSB.println(gnss.latitude, 6);

  SerialUSB.println("Write file...");
  myFile = SD.open(gpsFileName, FILE_WRITE);
  if(myFile){
    myFile.print(gnss.longitude);
    myFile.print(" ");
    myFile.println(gnss.latitude);
    myFile.println();
  }
  myFile.close();

  SeeedOled.setTextXY(0,0);          //Set the cursor to Xth Page, Yth Column  
  SeeedOled.putFloat(gnss.longitude, 6); //Print the String
  SeeedOled.setTextXY(2,0);          //Set the cursor to Xth Page, Yth Column  
  SeeedOled.putFloat(gnss.latitude, 6); //Print the String

  delay(2000);
}
