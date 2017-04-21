/********************************************************************************************
 This sketch demonstrates how sleep function using external interrupt to wake up the MCU.
 This sketch work only cortex-m0+ boards.
*********************************************************************************************/



#include <EnergySaving.h>
#include <Adafruit_NeoPixel.h>
#include "MC20_Common.h"
#include "MC20_Arduino_Interface.h"
#include "MC20_GNSS.h"

#define RGB_PIN 10
#define NUMPIXELS      1

#define DTR_PIN 9

int delayval = 500;

GNSS gnss = GNSS();

EnergySaving nrgSave;

unsigned int i=0;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, RGB_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
 for(int i = 0; i< 20; i++){
   pinMode(i, OUTPUT);
   digitalWrite(i, LOW);  
 }

 //digitalWrite(12, HIGH);
//  
//  digitalWrite(12, LOW);
//  digitalWrite(13, LOW);
//  digitalWrite(7, LOW);
//  digitalWrite(10, LOW);
  
  pinMode(DTR_PIN, OUTPUT);
  digitalWrite(DTR_PIN, HIGH);
  
  gnss.Power_On();
  SerialUSB.println("\n\rPower On!");

  if(!gnss.open_GNSS(GNSS_DEFAULT_MODE)){
    return;
  }

  delay(2000);

  pixels.begin();
  nrgSave.begin(WAKE_EXT_INTERRUPT, 3, dummy);  //standby setup for external interrupts
}

void loop()
{
  bool ret; 

  for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0,4,0)); // Moderately bright green color.

    pixels.show(); // This sends the updated pixel color to the hardware.

    delay(delayval); // Delay for a period of time (in milliseconds).

  }

  delay(2000);
  
  for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright green color.

    pixels.show(); // This sends the updated pixel color to the hardware.

    delay(delayval); // Delay for a period of time (in milliseconds).

  }
  
  // 1. GSM Least consumption
  // SerialUSB.print("Power DOWN: ");
  // SerialUSB.println(MC20_check_with_cmd("AT+QPOWD=1\r\n\"", "NORMAL POWER DOWN", CMD, 10), DEC);

  // delay(5000);
  // SerialUSB.print("POWERKEY DOWN！");
  // gnss.powerReset();
  // delay(5000);


  ret = gnss.setStandbyMode(0);
  SerialUSB.print("GNSS Stanby: ");
  SerialUSB.println(ret, DEC);
  // delay(5000);

  // ret = gnss.close_GNSS();
  // SerialUSB.print("GNSS close: ");
  // SerialUSB.println(ret, DEC);
  // digitalWrite(12, LOW);

  // 1.GSM function
  SerialUSB.println("GSM function...");
  ret = gnss.GSM_work_mode(0); 
  // ret = MC20_check_with_cmd("AT+QGNSSCMD=0,\"$PMTK161,0*28\"", "OK", CMD, 5, 2000); 
  SerialUSB.print("GSM least function Mode: "); //最少功能
  //ret = gnss.GSM_work_mode(1);  
  //SerialUSB.println("GSM all on Mode ..."); // 全功能
  //ret = gnss.GSM_work_mode(4);  
  //SerialUSB.println("GSM close RF Mode ..."); //关闭RF

  SerialUSB.println(ret, DEC);

  // delay(5000);
  
  // 3. GSM sleep Mode
  gnss.GSM_sleep_mode(1);  // sleep mode
  digitalWrite(9, HIGH);  
  SerialUSB.println("GSM Sleep Mode ...");
  // delay(5000);

  SerialUSB.println("MCU Stanby...");

  // for(int i = 0; i< 20; i++){
  //   if(i == 12 || i == 13 || i == 7){
  //     digitalWrite(12, HIGH);
  //     digitalWrite(13, LOW);
  //     digitalWrite(7, HIGH);
  //   }else{
  //     pinMode(i, INPUT);   
  //   }
  // }

  // pinMode(1, OUTPUT);
  // digitalWrite(1, LOW);
  // pinMode(8, OUTPUT);
  // digitalWrite(8, LOW);
  nrgSave.standby();  //now mcu goes in standby mode
}


void dummy(void)  //interrupt routine (isn't necessary to execute any tasks in this routine
{
  
}
