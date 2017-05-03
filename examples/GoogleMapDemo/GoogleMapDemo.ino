#include "MC20_Common.h"
#include "MC20_Arduino_Interface.h"
#include "MC20_GPRS.h"
#include "MC20_GNSS.h"
#include <SeeedOLED.h>
#include <Wire.h>

#define RGBPIN 10

GPRS gprs = GPRS();
GNSS gnss = GNSS();

const char apn[10] = "CMNET";
const char URL[100] = "ziladuo.com";
int port = 3001;

int ret = 0;

void setup() {
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);
  pinMode(RGBPIN, OUTPUT);
  digitalWrite(RGBPIN, LOW);
  SerialUSB.begin(115200);

  gprs.Power_On();
  SerialUSB.println("\n\rPower On!");

  /**
   * Init OLED
   */
  Wire.begin();
  SeeedOled.init();  //initialze SEEED OLED display
  SeeedOled.clearDisplay();          //clear the screen and set start position to top left corner
  SeeedOled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();           //Set addressing mode to Page Mode
  oled_log("Wio Tracker!");

  /**
   * Init GNSS
   */
  if(!gnss.open_GNSS(EPO_QUICK_MODE)){
    SerialUSB.println("Open GNSS failed!");
    oled_log("Open GNSS failed!");
    return;
  }
  SerialUSB.println("Open GNSS OK.");
  oled_log("Open GNSS O.K!");
  // delay(2000);

  /**
   * Init GPRS
   */
  if(!(ret = gprs.init(apn))) {
    SerialUSB.print("GPRS init error: ");
    SerialUSB.println(ret);
    oled_log("GPRS init failed!");
  }
  oled_log("GPRS init O.K!");

  gprs.join();
  SerialUSB.print("\n\rIP: ");
  SerialUSB.print(gprs.ip_string);

}

void loop() 
{
  static uint32_t searchCnt = 0;
  /**
   * Get GPS coordinate
   */
  // char buffer[64];
  if(gnss.getCoordinate())
  {
    SerialUSB.print("GNSS: ");
    SerialUSB.print(gnss.longitude, 6);
    SerialUSB.print(",");
    SerialUSB.println(gnss.latitude, 6);
    SerialUSB.print(gnss.str_longitude);
    SerialUSB.print(",");
    SerialUSB.println(gnss.str_latitude);

    if(gnss.longitude == 0 && gnss.latitude == 0)
    {
      searchCnt++;
      if(searchCnt > 100){
        searchCnt = 0;
      }
      SeeedOled.setTextXY(0,0);
      SeeedOled.putString("                ");
      SeeedOled.setTextXY(0,0);
      SeeedOled.putString("Seaercing GPS");
      SeeedOled.setTextXY(1,0);
      SeeedOled.putString("                ");
      SeeedOled.setTextXY(1,0);
      SeeedOled.putNumber(searchCnt);
    }
    else
    {
      // Coordinate display to OLED
      SeeedOled.setTextXY(0,0);
      SeeedOled.putString("                ");
      SeeedOled.setTextXY(0,0);
      SeeedOled.putString(gnss.str_latitude);  
      SeeedOled.setTextXY(1,0);
      SeeedOled.putString("                ");
      SeeedOled.setTextXY(1,0);
      SeeedOled.putString(gnss.str_longitude);
    }
  }
  else
  {
    SerialUSB.println("Error!");
    SeeedOled.setTextXY(0,0);
    SeeedOled.putString("                ");
    SeeedOled.setTextXY(0,0);
    SeeedOled.putString("Running error!");
  }
  postCoordiante(); 
  delay(1000);
}

void oled_log(char *str)
{
  SeeedOled.setTextXY(1,0);
  SeeedOled.putString("                "); 
  SeeedOled.setTextXY(1,0);
  SeeedOled.putString(str); 
}

/**
 * POST to server
 */
void postCoordiante()
{
  if(gprs.connectTCP(URL, port))
  {
    bool ret;
    char postContent[128];

    /**
      * Load latlng data to postContent
     */
    MC20_clean_buffer(postContent, 100);
    sprintf(postContent, "POST /api/latlng/%lu&%lu/ HTTP/1.0\n\r\n\r", (uint32_t)(gnss.latitude*10000000), (uint32_t)(gnss.longitude*10000000));
    SerialUSB.print(postContent);
    gprs.sendTCPData(postContent);   // Send HTTP request
    ret = MC20_wait_for_resp("CLOSED\r\n", CMD, 10, 2000, true);
    SerialUSB.print("Wait for request: ");
    SerialUSB.println(ret, DEC);
  } 
  else
  {
    SerialUSB.println("Connect Error!");
  }
}