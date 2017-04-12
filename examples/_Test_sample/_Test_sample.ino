#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <TimerTCC0.h>

#define PWR_GNSS 7
#define PWR_KEY 13
#define DEBUG true

enum DataType {
    CMD     = 0,
    DATA    = 1,
};

int led_red = 2;
static int state = 0;

int MC20_check_readable();
boolean MC20_wait_for_resp(const char* resp, DataType type, unsigned int timeout, unsigned int chartimeout);
void MC20_flush_serial();
boolean MC20_check_with_cmd(const char* cmd, const char *resp, DataType type, unsigned int timeout, unsigned int chartimeout);
void MC20_send_cmd(const char* cmd);


int MC20_open_GPS(void)
{
  if(!MC20_check_with_cmd("AT+QGNSSC?\n\r", "+QGNSSC: 1", CMD, 2, 2000)){
      MC20_check_with_cmd("AT+QGNSSC=1\n\r", "OK", CMD, 2, 2000);  
  }

  delay(500);

  return 0;
}

// int MC20_Test_GSM(String PhoenNumber)
int MC20_Test_GSM(void)
{
  
  // SerialUSB.print("Dial Number: ");
  // SerialUSB.println(PhoenNumber);
  while(0 != Check_If_Call_Ready())
  {
    SerialUSB.println("Call Not Ready");
    delay(1000);
  }

  SerialUSB.print("Call: ");
  SerialUSB.println(MC20_check_with_cmd("ATD15018657860;\r\n", "OK", CMD, 2, 5000), DEC);

  delay(15000);
  Exit_A_Call();
  return 0;
}

int MC20_Test_Storage(void)
{

}

void timerIsr()
{ 
   digitalWrite(led_red, state);
   state = !state;
   SerialUSB.println("ISR");
}


void setup() {
  SerialUSB.begin(115200);
  // while(!SerialUSB);
  Serial1.begin(115200);
  SerialUSB.println("Start...");
  MC20_Power_On();

  pinMode(led_red, OUTPUT);
  digitalWrite(led_red, LOW);

  initialize();

  // Wait for GSM to connect to GSM network
  delay(10000);

  
  //MC20_Test_GSM();
  MC20_open_GPS();
  
  // TimerTcc0.initialize(1000000);
  // TimerTcc0.attachInterrupt(timerIsr);
}


void loop() {

  /* Debug */
  /*if(SerialUSB.available()){
    Serial1.write(SerialUSB.read());
  }
  if(Serial1.available()){     
    SerialUSB.write(Serial1.read()); 
  }*/

  /* Test GPS */
  if(MC20_check_with_cmd("AT+QGNSSRD?\n\r", "OK", CMD, 2, 2000))
   {
     digitalWrite(led_red, state);
     state = !state;
   }
   delay(1000);

}

int initialize(void)
{
  while(1){
    boolean stat = MC20_check_with_cmd("AT\n\r", "OK", CMD, 2, 2000);
    if(stat==true) break;
    SerialUSB.print("Checking AT CMD...");
    delay(1000);
  } 
}

int MC20_Check_If_Power_On(void)
{
  boolean state = MC20_check_with_cmd("AT\n\r", "OK", CMD, 2, 2000);
  if(state){
    return 0;
  } else {
    return -1;
  }
}
int MC20_Power_On(void)
{
  int state = MC20_Check_If_Power_On();
  if(0 == state){
    return 0;
  }

  pinMode(PWR_GNSS, OUTPUT);
  pinMode(PWR_KEY, OUTPUT);
  //pinMode(SLEEP_PIN, OUTPUT);
  
  digitalWrite(PWR_KEY, LOW); 
  delay(2000);
  //digitalWrite(SLEEP_PIN, LOW); 
  digitalWrite(PWR_GNSS, LOW); 
  //digitalWrite(RST_PIN, LOW); 
  

  digitalWrite(PWR_KEY, HIGH); 
  delay(2000);
  digitalWrite(PWR_KEY, LOW); 
  digitalWrite(PWR_GNSS, HIGH);
}

/* Phone call methods */
int Check_If_Call_Ready(void)
{

  boolean stat = MC20_check_with_cmd("AT+CPIN?\n\r", "+CPIN: READY", CMD, 2, 2000);
  SerialUSB.print("Check Call Ready: ");
  SerialUSB.println(stat, DEC);
  if(!stat) return -1;
  return 0;
}

int Exit_A_Call(void)
{
  boolean stat = MC20_check_with_cmd("ATH\n\r", "OK", CMD, 2, 2000);
  SerialUSB.print("Exit A Call, ");
  SerialUSB.println(stat, DEC);
  if(!stat) return -1;
  return 0;
}

/* End of Phone call methods */
int MC20_check_readable()
{
    return Serial1.available();
}

boolean MC20_wait_for_resp(const char* resp, DataType type, unsigned int timeout, unsigned int chartimeout)
{
    int len = strlen(resp);
    int sum = 0;
    unsigned long timerStart, prevChar;    //prevChar is the time when the previous Char has been read.
    timerStart = millis();
    prevChar = 0;
    while(1) {
        if(MC20_check_readable()) {
            char c = Serial1.read();
            /* Debug */
            SerialUSB.write(c);

            prevChar = millis();
            sum = (c==resp[sum]) ? sum+1 : 0;
            
            /* Debug */
            // SerialUSB.print("Check len: ");
            // SerialUSB.print(len);
            // SerialUSB.print(" Check sum: ");
            // SerialUSB.println(sum);

            if(sum == len)break;
        }
        if ((unsigned long) (millis() - timerStart) > timeout * 1000UL) {
            return false;
        }
        //If interchar Timeout => return FALSE. So we can return sooner from this function.
        if (((unsigned long) (millis() - prevChar) > chartimeout) && (prevChar != 0)) {
            return false;
        }

    }

    //If is a CMD, we will finish to read buffer.
    if(type == CMD) MC20_flush_serial();
    return true;
}
  
void MC20_flush_serial()
{
    while(MC20_check_readable()){
        Serial1.read();
    }
} 

boolean MC20_check_with_cmd(const char* cmd, const char *resp, DataType type, unsigned int timeout, unsigned int chartimeout)
{
    MC20_send_cmd(cmd);
    return MC20_wait_for_resp(resp,type,timeout,chartimeout);
}

void MC20_send_cmd(const char* cmd){
  Serial1.println(cmd);
}
