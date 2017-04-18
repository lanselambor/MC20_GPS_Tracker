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

int grovePins[10] = {2,3,4,5,14,15,16,17,20,21};
int pinIndex = 0;

void setup() {
  for(pinIndex = 0; pinIndex < 10; pinIndex++){
    pinMode(grovePins[pinIndex], OUTPUT);
    digitalWrite(grovePins[pinIndex], LOW);
  }

  SerialDBG.begin(115200);
  // SerialDBG.setTimeout(10);
  SerialUSB.begin(115200);
  // while(!SerialUSB);

  gpsTracker.Power_On();
  SerialUSB.println("Power On!");

  while(!gpsTracker.init()){
    delay(1000);
    SerialUSB.println("Accessing network...");
  }

  // Make a phone call
  gpsTracker.callUp("xxxxxxxxxxx");
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
  static int pinState = LOW;
  for(pinIndex = 0; pinIndex < 10; pinIndex++){
    digitalWrite(grovePins[pinIndex], pinState);
  }
  pinState = ~pinState;
  
  // Test SerialDBG
  SerialDBG.write('a');
  delay(1);
  if('a' == SerialDBG.read()){
    SerialUSB.println("Grove UART Test O.K!");
  } else {
    SerialUSB.println("Grove UART Test Failed!");
  }

  gnss.dataFlowMode();
  delay(1000);
}
