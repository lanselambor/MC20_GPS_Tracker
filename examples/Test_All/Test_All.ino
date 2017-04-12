#include "MC20_Common.h"
#include "MC20_Arduino_Interface.h"
#include "MC20_BT.h"
#include "MC20_Common.h"
#include "MC20_GNSS.h"
#include "MC20_GPRS.h"


GPSTracker gpsTracker = GPSTracker();
BlueTooth bt = BlueTooth();
int bt_state = -1;
char *deviceName = "N-612";
GNSS gnss = GNSS();

void setup() {
  SerialUSB.begin(115200);
  // while(!SerialUSB);

  gpsTracker.Power_On();
  SerialUSB.println("Power On!");

  while(!gpsTracker.init()){
    delay(1000);
    SerialUSB.println("Accessing network...");
  }

  // Make a phone call
  gpsTracker.callUp("13750024343");
  SerialUSB.println("Calling...");

  bt.BTPowerOn();
  SerialUSB.println("\n\rBT power On!");

  while(IDLE != (bt_state = bt.getBTState())){
    SerialUSB.print("State: ");
    SerialUSB.println(bt_state);
    delay(1000);
  }

  bt.BTFastConnect(deviceName, HFG_PROFILE);

  while(!gnss.open_GNSS()){
    delay(1000);
  }
}

void loop() {
  gnss.dataFlowMode();
  delay(2000);
}
