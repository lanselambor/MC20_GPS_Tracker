/***********************************************************************************************
 This sketch demonstrates how to use sleep features and wake-up MCU with RTC alarm interrupt.
 This sketch works only on cortex-m0+ based boards.
************************************************************************************************/

#include <EnergySaving.h>
#include <RTCInt.h>
#include <Adafruit_NeoPixel.h>

#define PIN 10
#define NUMPIXELS      1

EnergySaving nrgSave;
RTCInt rtc;

unsigned int i=0;

int delayval = 500;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  SerialUSB.begin(115200);   //serial settings
  
  pixels.begin();

  rtc.begin(TIME_H24);
  rtc.setTime(12,48,0,0);
  rtc.setDate(22,10,15);
  rtc.enableAlarm(SEC,ALARM_INTERRUPT,rest_alarm_int);
  rtc.time.hour=12;
  rtc.time.minute=48;
  rtc.time.second=2;
  nrgSave.begin(WAKE_RTC_ALARM);  //standby setup for external interrupts
  rtc.setAlarm();
  SerialUSB.println("START");
}

void loop()
{
  for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0,4,0)); // Moderately bright green color.

    pixels.show(); // This sends the updated pixel color to the hardware.

    delay(delayval); // Delay for a period of time (in milliseconds).

  }

  delay(1000);

  for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright green color.

    pixels.show(); // This sends the updated pixel color to the hardware.

    delay(delayval); // Delay for a period of time (in milliseconds).

  }
  SerialUSB.println("_START");
  nrgSave.standby();  //now mcu go to standby
}

void rest_alarm_int(void)  //interrupt routine
{
  for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0,4,0)); // Moderately bright green color.

    pixels.show(); // This sends the updated pixel color to the hardware.

    delay(delayval); // Delay for a period of time (in milliseconds).

  }

  delay(1000);

  for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright green color.

    pixels.show(); // This sends the updated pixel color to the hardware.

    delay(delayval); // Delay for a period of time (in milliseconds).

  }
  rtc.setTime(12,48,0,0);
  SerialUSB.println("START");

}