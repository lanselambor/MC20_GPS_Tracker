
#include "MC20_Arduino_Interface.h"

#define GrovePowerPin   12
#define LightSensorPin  A0
#define ButtonPin       2


void setup() {
  SerialUSB.begin(115200);
  pinMode(ButtonPin, INPUT);
  pinMode(GrovePowerPin, OUTPUT);
  digitalWrite(GrovePowerPin, HIGH);   //write high to grove power pin to open power switch of grove port
}

void loop() {
    if (digitalRead(ButtonPin)) {
        SerialUSB.print("The value of light sensor:");
        SerialUSB.println(analogRead(LightSensorPin));
    }
    delay(500);
}   
