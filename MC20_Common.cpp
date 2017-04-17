/*
 * MC20_Arduino_Interface.cpp
 * A library for SeeedStudio GPS Tracker
 *  
 * Copyright (c) 2017 seeed technology inc.
 * Website    : www.seeed.cc
 * Author     : lawliet zou, lambor
 * Create Time: April 2017
 * Change Log :
 *
 * The MIT License (MIT)
  *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

 #include <stdio.h>
 #include "MC20_Common.h"

// GPSTracker* GPSTracker::inst;

GPSTracker::GPSTracker()
{
    // inst = this;
    // MC20_init();
    pinMode(RGB_PIN, OUTPUT);
    digitalWrite(RGB_PIN, LOW);
}

bool GPSTracker::init(void)
{
    if(!MC20_check_with_cmd(F("AT\r\n"),"OK\r\n",CMD)){
        return false;
    }
    
    if(!MC20_check_with_cmd(F("AT+CPIN?\r\n"),"OK\r\n",CMD)){
        return false;
    }
    
    if(!checkSIMStatus()) {
        return false;
    }
    return true;
}

bool GPSTracker::Check_If_Power_On(void)
{
  return MC20_check_with_cmd(F("AT\n\r"), "OK", CMD, 2, 2000);
}

void GPSTracker::Power_On(void)
{
  MC20_init();

  if(Check_If_Power_On()){
    return;
  }

  pinMode(PWR_GNSS, OUTPUT);
  pinMode(PWR_KEY, OUTPUT);
  digitalWrite(PWR_KEY, LOW); 
  delay(2000);
  digitalWrite(PWR_GNSS, LOW);
  digitalWrite(PWR_KEY, HIGH);
  delay(2000);
  digitalWrite(PWR_GNSS, HIGH);
}

void GPSTracker::powerReset(void)
{ 
  digitalWrite(PWR_KEY, LOW);
}
  
  
bool GPSTracker::checkSIMStatus(void)
{
    char mc20_Buffer[32];
    int count = 0;
    MC20_clean_buffer(mc20_Buffer,32);
    while(count < 3) {
        MC20_send_cmd("AT+CPIN?\r\n");
        MC20_read_buffer(mc20_Buffer,32,DEFAULT_TIMEOUT);
        if((NULL != strstr(mc20_Buffer,"+CPIN: READY"))) {
            break;
        }
        count++;
        delay(300);
    }
    if(count == 3) {
        return false;
    }
    return true;
}

bool GPSTracker::sendSMS(char *number, char *data)
{
    //char cmd[32];
    if(!MC20_check_with_cmd(F("AT+CMGF=1\r\n"), "OK\r\n", CMD)) { // Set message mode to ASCII
        return false;
    }
    delay(500);
    MC20_flush_serial();
    MC20_send_cmd("AT+CMGS=\"");
    MC20_send_cmd(number);
    //sprintf(cmd,"AT+CMGS=\"%s\"\r\n", number);
    //snprintf(cmd, sizeof(cmd),"AT+CMGS=\"%s\"\r\n", number);
//    if(!MC20_check_with_cmd(cmd,">",CMD)) {
    if(!MC20_check_with_cmd(F("\"\r\n"),">",CMD)) {
        return false;
    }
    delay(1000);
    MC20_send_cmd(data);
    delay(500);
    MC20_send_End_Mark();
    return MC20_wait_for_resp("OK\r\n", CMD);
}

char GPSTracker::isSMSunread()
{
    char mc20_Buffer[48];  //48 is enough to see +CMGL:
    char *s;
    

    //List of all UNREAD SMS and DON'T change the SMS UNREAD STATUS
    MC20_send_cmd("AT+CMGL=\"REC UNREAD\",1\r\n");
    /*If you want to change SMS status to READ you will need to send:
          AT+CMGL=\"REC UNREAD\"\r\n
      This command will list all UNREAD SMS and change all of them to READ
      
     If there is not SMS, response is (30 chars)
         AT+CMGL="REC UNREAD",1  --> 22 + 2
                                 --> 2
         OK                      --> 2 + 2

     If there is SMS, response is like (>64 chars)
         AT+CMGL="REC UNREAD",1
         +CMGL: 9,"REC UNREAD","XXXXXXXXX","","14/10/16,21:40:08+08"
         Here SMS text.
         OK  
         
         or

         AT+CMGL="REC UNREAD",1
         +CMGL: 9,"REC UNREAD","XXXXXXXXX","","14/10/16,21:40:08+08"
         Here SMS text.
         +CMGL: 10,"REC UNREAD","YYYYYYYYY","","14/10/16,21:40:08+08"
         Here second SMS        
         OK           
    */

    MC20_clean_buffer(mc20_Buffer,31); 
    MC20_read_buffer(mc20_Buffer,30,DEFAULT_TIMEOUT); 
    //Serial.print("Buffer isSMSunread: ");Serial.println(mc20_Buffer);

    if(NULL != ( s = strstr(mc20_Buffer,"OK"))) {
        //In 30 bytes "doesn't" fit whole +CMGL: response, if recieve only "OK"
        //    means you don't have any UNREAD SMS
        delay(50);
        return 0;
    } else {
        //More buffer to read
        //We are going to flush serial data until OK is recieved
        MC20_wait_for_resp("OK\r\n", CMD);        
        //MC20_flush_serial();
        //We have to call command again
        MC20_send_cmd("AT+CMGL=\"REC UNREAD\",1\r\n");
        MC20_clean_buffer(mc20_Buffer,48); 
        MC20_read_buffer(mc20_Buffer,47,DEFAULT_TIMEOUT);
        //Serial.print("Buffer isSMSunread 2: ");Serial.println(mc20_Buffer);       
        if(NULL != ( s = strstr(mc20_Buffer,"+CMGL:"))) {
            //There is at least one UNREAD SMS, get index/position
            s = strstr(mc20_Buffer,":");
            if (s != NULL) {
                //We are going to flush serial data until OK is recieved
                MC20_wait_for_resp("OK\r\n", CMD);
                return atoi(s+1);
            }
        } else {
            return -1; 

        }
    } 
    return -1;
}

bool GPSTracker::readSMS(int messageIndex, char *message, int length, char *phone, char *datetime)  
{
  /* Response is like:
  AT+CMGR=2
  
  +CMGR: "REC READ","XXXXXXXXXXX","","14/10/09,17:30:17+08"
  SMS text here
  
  So we need (more or lees), 80 chars plus expected message length in buffer. CAUTION FREE MEMORY
  */

    int i = 0;
    char mc20_Buffer[80 + length];
    //char cmd[16];
    char num[4];
    char *p,*p2,*s;
    
    MC20_check_with_cmd(F("AT+CMGF=1\r\n"),"OK\r\n",CMD);
    delay(1000);
    //sprintf(cmd,"AT+CMGR=%d\r\n",messageIndex);
    //MC20_send_cmd(cmd);
    MC20_send_cmd("AT+CMGR=");
    itoa(messageIndex, num, 10);
    MC20_send_cmd(num);
    MC20_send_cmd("\r\n");
    MC20_clean_buffer(mc20_Buffer,sizeof(mc20_Buffer));
    MC20_read_buffer(mc20_Buffer,sizeof(mc20_Buffer));
      
    if(NULL != ( s = strstr(mc20_Buffer,"READ\",\""))){
        // Extract phone number string
        p = strstr(s,",");
        p2 = p + 2; //We are in the first phone number character
        p = strstr((char *)(p2), "\"");
        if (NULL != p) {
            i = 0;
            while (p2 < p) {
                phone[i++] = *(p2++);
            }
            phone[i] = '\0';            
        }
        // Extract date time string
        p = strstr((char *)(p2),",");
        p2 = p + 1; 
        p = strstr((char *)(p2), ","); 
        p2 = p + 2; //We are in the first date time character
        p = strstr((char *)(p2), "\"");
        if (NULL != p) {
            i = 0;
            while (p2 < p) {
                datetime[i++] = *(p2++);
            }
            datetime[i] = '\0';
        }        
        if(NULL != ( s = strstr(s,"\r\n"))){
            i = 0;
            p = s + 2;
            while((*p != '\r')&&(i < length-1)) {
                message[i++] = *(p++);
            }
            message[i] = '\0';
        }
        return true;
    }
    return false;    
}

bool GPSTracker::readSMS(int messageIndex, char *message,int length)
{
    int i = 0;
    char mc20_Buffer[100];
    //char cmd[16];
    char num[4];
    char *p,*s;
    
    MC20_check_with_cmd(F("AT+CMGF=1\r\n"),"OK\r\n",CMD);
    delay(1000);
    MC20_send_cmd("AT+CMGR=");
    itoa(messageIndex, num, 10);
    MC20_send_cmd(num);
    // MC20_send_cmd("\r\n");
    MC20_send_cmd("\r\n");
//  sprintf(cmd,"AT+CMGR=%d\r\n",messageIndex);
//    MC20_send_cmd(cmd);
    MC20_clean_buffer(mc20_Buffer,sizeof(mc20_Buffer));
    MC20_read_buffer(mc20_Buffer,sizeof(mc20_Buffer),DEFAULT_TIMEOUT);
    if(NULL != ( s = strstr(mc20_Buffer,"+CMGR:"))){
        if(NULL != ( s = strstr(s,"\r\n"))){
            p = s + 2;
            while((*p != '\r')&&(i < length-1)) {
                message[i++] = *(p++);
            }
            message[i] = '\0';
            return true;
        }
    }
    return false;   
}

bool GPSTracker::deleteSMS(int index)
{
    //char cmd[16];
    char num[4];
    //sprintf(cmd,"AT+CMGD=%d\r\n",index);
    MC20_send_cmd("AT+CMGD=");
    itoa(index, num, 10);
    MC20_send_cmd(num);
    //snprintf(cmd,sizeof(cmd),"AT+CMGD=%d\r\n",index);
    //MC20_send_cmd(cmd);
    //return 0;
    // We have to wait OK response
    //return MC20_check_with_cmd(cmd,"OK\r\n",CMD);
    return MC20_check_with_cmd(F("\r"),"OK\r\n",CMD); 
}

bool GPSTracker::callUp(char *number)
{
    //char cmd[24];
    if(!MC20_check_with_cmd(F("AT+COLP=1\r\n"),"OK\r\n",CMD)) {
        return false;
    }
    delay(1000);
    //HACERR quitar SPRINTF para ahorar memoria ???
    //sprintf(cmd,"ATD%s;\r\n", number);
    //MC20_send_cmd(cmd);
    MC20_send_cmd("ATD");
    MC20_send_cmd(number);
    MC20_send_cmd(";\r\n");
    return true;
}

void GPSTracker::answer(void)
{
    MC20_send_cmd("ATA\r\n");  //TO CHECK: ATA doesnt return "OK" ????
}

bool GPSTracker::hangup(void)
{
    return MC20_check_with_cmd(F("ATH\r\n"),"OK\r\n",CMD);
}

bool GPSTracker::disableCLIPring(void)
{
    return MC20_check_with_cmd(F("AT+CLIP=0\r\n"),"OK\r\n",CMD);
}

bool GPSTracker::getSubscriberNumber(char *number)
{
    //AT+CNUM                               --> 7 + CR = 8
    //+CNUM: "","+628157933874",145,7,4     --> CRLF + 45 + CRLF = 49
    //                                      -->
    //OK                                    --> CRLF + 2 + CRLF = 6

    byte i = 0;
    char mc20_Buffer[65];
    char *p,*s;
    MC20_flush_serial();
    MC20_send_cmd("AT+CNUM\r\n");
    MC20_clean_buffer(mc20_Buffer,65);
    MC20_read_buffer(mc20_Buffer,65,DEFAULT_TIMEOUT);
    //Serial.print(mc20_Buffer);
    if(NULL != ( s = strstr(mc20_Buffer,"+CNUM:"))) {
        s = strstr((char *)(s),",");
        s = s + 2;  //We are in the first phone number character 
        p = strstr((char *)(s),"\""); //p is last character """
        if (NULL != s) {
            i = 0;
            while (s < p) {
              number[i++] = *(s++);
            }
            number[i] = '\0';
        }
        return true;
    }  
    return false;
}

bool GPSTracker::isCallActive(char *number)
{
    char mc20_Buffer[46];  //46 is enough to see +CPAS: and CLCC:
    char *p, *s;
    int i = 0;

    MC20_send_cmd("AT+CPAS\r\n");
    /*Result code:
        0: ready
        2: unknown
        3: ringing
        4: call in progress
    
      AT+CPAS   --> 7 + 2 = 9 chars
                --> 2 char              
      +CPAS: 3  --> 8 + 2 = 10 chars
                --> 2 char
      OK        --> 2 + 2 = 4 chars
    
      AT+CPAS
      
      +CPAS: 0
      
      OK
    */

    MC20_clean_buffer(mc20_Buffer,29);
    MC20_read_buffer(mc20_Buffer,27);
    //HACERR cuando haga lo de esperar a OK no me haría falta esto
    //We are going to flush serial data until OK is recieved
    MC20_wait_for_resp("OK\r\n", CMD);    
    //Serial.print("Buffer isCallActive 1: ");Serial.println(mc20_Buffer);
    if(NULL != ( s = strstr(mc20_Buffer,"+CPAS:"))) {
      s = s + 7;
      if (*s != '0') {
         //There is something "running" (but number 2 that is unknow)
         if (*s != '2') {
           //3 or 4, let's go to check for the number
           MC20_send_cmd("AT+CLCC\r\n");
           /*
           AT+CLCC --> 9
           
           +CLCC: 1,1,4,0,0,"656783741",161,""
           
           OK  

           Without ringing:
           AT+CLCC
           OK              
           */

           MC20_clean_buffer(mc20_Buffer,46);
           MC20_read_buffer(mc20_Buffer,45);
            //Serial.print("Buffer isCallActive 2: ");Serial.println(mc20_Buffer);
           if(NULL != ( s = strstr(mc20_Buffer,"+CLCC:"))) {
             //There is at least one CALL ACTIVE, get number
             s = strstr((char *)(s),"\"");
             s = s + 1;  //We are in the first phone number character            
             p = strstr((char *)(s),"\""); //p is last character """
             if (NULL != s) {
                i = 0;
                while (s < p) {
                    number[i++] = *(s++);
                }
                number[i] = '\0';            
             }
             //I need to read more buffer
             //We are going to flush serial data until OK is recieved
             return MC20_wait_for_resp("OK\r\n", CMD); 
           }
         }
      }        
    } 
    return false;
}

bool GPSTracker::getDateTime(char *buffer)
{
    //If it doesn't work may be for two reasons:
    //      1. Your carrier doesn't give that information
    //      2. You have to configurate the SIM900 IC.
    //          - First with SIM900_Serial_Debug example try this AT command: AT+CLTS?
    //          - If response is 0, then it is disabled.
    //          - Enable it by: AT+CLTS=1
    //          - Now you have to save this config to EEPROM memory of SIM900 IC by: AT&W
    //          - Now, you have to power down and power up again the SIM900 
    //          - Try now again: AT+CCLK?
    //          - It should work
    
    //AT+CCLK?                      --> 8 + CR = 9
    //+CCLK: "14/11/13,21:14:41+04" --> CRLF + 29+ CRLF = 33
    //                              
    //OK                            --> CRLF + 2 + CRLF =  6

    byte i = 0;
    char mc20_Buffer[50];
    char *p,*s;
    MC20_flush_serial();
    MC20_send_cmd("AT+CCLK?\r");
    MC20_clean_buffer(mc20_Buffer,50);
    MC20_read_buffer(mc20_Buffer,50,DEFAULT_TIMEOUT);
    if(NULL != ( s = strstr(mc20_Buffer,"+CCLK:"))) {
        s = strstr((char *)(s),"\"");
        s = s + 1;  //We are in the first phone number character 
        p = strstr((char *)(s),"\""); //p is last character """
        if (NULL != s) {
            i = 0;
            while (s < p) {
              buffer[i++] = *(s++);
            }
            buffer[i] = '\0';            
        }
        return true;
    }  
    return false;
}

bool GPSTracker::getSignalStrength(int *buffer)
{
    //AT+CSQ                        --> 6 + CR = 10
    //+CSQ: <rssi>,<ber>            --> CRLF + 5 + CRLF = 9                     
    //OK                            --> CRLF + 2 + CRLF =  6

    byte i = 0;
    char mc20_Buffer[26];
    char *p, *s;
    char buffers[4];
    MC20_flush_serial();
    MC20_send_cmd("AT+CSQ\r");
    MC20_clean_buffer(mc20_Buffer, 26);
    MC20_read_buffer(mc20_Buffer, 26, DEFAULT_TIMEOUT);
    if (NULL != (s = strstr(mc20_Buffer, "+CSQ:"))) {
        s = strstr((char *)(s), " ");
        s = s + 1;  //We are in the first phone number character 
        p = strstr((char *)(s), ","); //p is last character """
        if (NULL != s) {
            i = 0;
            while (s < p) {
                buffers[i++] = *(s++);
            }
            buffers[i] = '\0';
        }
        *buffer = atoi(buffers);
        return true;
    }
    return false;
}

bool GPSTracker::sendUSSDSynchronous(char *ussdCommand, char *resultcode, char *response)
{
    //AT+CUSD=1,"{command}"
    //OK
    //
    //+CUSD:1,"{response}",{int}

    byte i = 0;
    char mc20_Buffer[200];
    char *p,*s;
    MC20_clean_buffer(response, sizeof(response));
    
    MC20_flush_serial();
    MC20_send_cmd("AT+CUSD=1,\"");
    MC20_send_cmd(ussdCommand);
    MC20_send_cmd("\"\r");
    if(!MC20_wait_for_resp("OK\r\n", CMD))
        return false;
    MC20_clean_buffer(mc20_Buffer,200);
    MC20_read_buffer(mc20_Buffer,200,DEFAULT_TIMEOUT);
    if(NULL != ( s = strstr(mc20_Buffer,"+CUSD: "))) {
        *resultcode = *(s+7);
        resultcode[1] = '\0';
        if(!('0' <= *resultcode && *resultcode <= '2'))
            return false;
        s = strstr(s,"\"");
        s = s + 1;  //We are in the first phone number character
        p = strstr(s,"\""); //p is last character """
        if (NULL != s) {
            i = 0;
            while (s < p) {
              response[i++] = *(s++);
            }
            response[i] = '\0';            
        }
        return true;
    }
    return false;
}

bool GPSTracker::cancelUSSDSession(void)
{
    return MC20_check_with_cmd(F("AT+CUSD=2\r\n"),"OK\r\n",CMD);
}

//Here is where we ask for APN configuration, with F() so we can save MEMORY
bool GPSTracker::join(const char *apn, const char *userName, const char *passWord)
{
    byte i;
    char *p, *s;
    char ipAddr[32];
    //Select multiple connection
    //MC20_check_with_cmd("AT+CIPMUX=1\r\n","OK",DEFAULT_TIMEOUT,CMD);

    //set APN. OLD VERSION
    //snprintf(cmd,sizeof(cmd),"AT+CSTT=\"%s\",\"%s\",\"%s\"\r\n",_apn,_userName,_passWord);
    //MC20_check_with_cmd(cmd, "OK\r\n", DEFAULT_TIMEOUT,CMD);
    MC20_send_cmd("AT+CSTT=\"");
    if (apn) {
      MC20_send_cmd(apn);
    }
    MC20_send_cmd("\",\"");
    if (userName) {
        MC20_send_cmd(userName);
    }
    MC20_send_cmd("\",\"");
    if (passWord) {
        MC20_send_cmd(passWord);
    }
    if (!MC20_check_with_cmd(F("\"\r\n"), "OK\r\n", CMD)) {
        return false;
    }
 

    //Brings up wireless connection
    if (!MC20_check_with_cmd(F("AT+CIICR\r\n"),"OK\r\n", CMD)) {
        return false;
    }
    
    //Get local IP address
    MC20_send_cmd("AT+CIFSR\r\n");
    MC20_clean_buffer(ipAddr,32);
    MC20_read_buffer(ipAddr,32);
    //Response:
    //AT+CIFSR\r\n       -->  8 + 2
    //\r\n               -->  0 + 2
    //10.160.57.120\r\n  --> 15 + 2 (max)   : TOTAL: 29 
    //Response error:
    //AT+CIFSR\r\n       
    //\r\n               
    //ERROR\r\n
    if (NULL != strstr(ipAddr,"ERROR")) {
        return false;
    }
    s = ipAddr + 11;
    p = strstr((char *)(s),"\r\n"); //p is last character \r\n
    if (NULL != s) {
        i = 0;
        while (s < p) {
            ip_string[i++] = *(s++);
        }
        ip_string[i] = '\0';            
    }
    _ip = str_to_ip(ip_string);
    if(_ip != 0) {
        return true;
    }
    return false;
} 

void GPSTracker::disconnect()
{
    MC20_check_with_cmd("AT+CIPSHUT\r\n", "SHUT OK\r\n", CMD);
}

bool GPSTracker::connect(Protocol ptl,const char * host, int port, int timeout, int chartimeout)
{
    //char cmd[64];
    char num[4];
    char resp[96];
    
    //MC20_clean_buffer(cmd,64);
    if(ptl == TCP) {
        MC20_send_cmd("AT+CIPSTART=\"TCP\",\"");
        MC20_send_cmd(host);
        MC20_send_cmd("\",");
        itoa(port, num, 10);
        MC20_send_cmd(num);
        MC20_send_cmd("\r\n");
//        sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",host, port);
    } else if(ptl == UDP) {
        MC20_send_cmd("AT+CIPSTART=\"UDP\",\"");
        MC20_send_cmd(host);
        MC20_send_cmd("\",");
        itoa(port, num, 10);
        MC20_send_cmd(num);
        MC20_send_cmd("\r\n");

    //        sprintf(cmd, "AT+CIPSTART=\"UDP\",\"%s\",%d\r\n",host, port);
    } else {
        return false;
    }
    

    //MC20_send_cmd(cmd);
    MC20_read_buffer(resp, 96, timeout, chartimeout);
    //Serial.print("Connect resp: "); Serial.println(resp);    
    if(NULL != strstr(resp,"CONNECT")) { //ALREADY CONNECT or CONNECT OK
        return true;
    }
    return false;
}

//Overload with F() macro to SAVE memory
bool GPSTracker::connect(Protocol ptl,const char *host, const char *port, int timeout, int chartimeout)
{

    char resp[96];


    if(ptl == TCP) {
        MC20_send_cmd("AT+CIPSTART=\"TCP\",\"");   //%s\",%d\r\n",host, port);
    } else if(ptl == UDP) {
        MC20_send_cmd("AT+CIPSTART=\"UDP\",\"");   //%s\",%d\r\n",host, port);
    } else {
        return false;
    }
    MC20_send_cmd(host);
    MC20_send_cmd("\",");
    MC20_send_cmd(port);
    MC20_send_cmd("\r\n");
//  Serial.print("Connect: "); Serial.println(cmd);
    MC20_read_buffer(resp, 96, timeout, chartimeout);
//  Serial.print("Connect resp: "); Serial.println(resp);    
    if(NULL != strstr(resp,"CONNECT")) { //ALREADY CONNECT or CONNECT OK
        return true;
    }
    return false;
}

bool GPSTracker::is_connected(void)
{
    char resp[96];
    MC20_send_cmd("AT+CIPSTATUS\r\n");
    MC20_read_buffer(resp,sizeof(resp),DEFAULT_TIMEOUT);
    if(NULL != strstr(resp,"CONNECTED")) {
        //+CIPSTATUS: 1,0,"TCP","216.52.233.120","80","CONNECTED"
        return true;
    } else {
        //+CIPSTATUS: 1,0,"TCP","216.52.233.120","80","CLOSED"
        //+CIPSTATUS: 0,,"","","","INITIAL"
        return false;
    }
}

bool GPSTracker::close()
{
    // if not connected, return
    if (!is_connected()) {
        return true;
    }
    return MC20_check_with_cmd("AT+CIPCLOSE\r\n", "CLOSE OK\r\n", CMD);
}

int GPSTracker::readable(void)
{
    return MC20_check_readable();
}

int GPSTracker::wait_readable(int wait_time)
{
    return MC20_wait_readable(wait_time);
}

int GPSTracker::wait_writeable(int req_size)
{
    return req_size+1;
}

int GPSTracker::send(const char * str, int len)
{
    //char cmd[32];
    char num[4];
    if(len > 0){
        //snprintf(cmd,sizeof(cmd),"AT+CIPSEND=%d\r\n",len);
        //sprintf(cmd,"AT+CIPSEND=%d\r\n",len);
        MC20_send_cmd("AT+CIPSEND=");
        itoa(len, num, 10);
        MC20_send_cmd(num);
        if(!MC20_check_with_cmd("\r\n",">",CMD)) {
        //if(!MC20_check_with_cmd(cmd,">",CMD)) {
            return 0;
        }
        /*if(0 != MC20_check_with_cmd(str,"SEND OK\r\n", DEFAULT_TIMEOUT * 10 ,DATA)) {
            return 0;
        }*/
        delay(500);
        MC20_send_cmd(str);
        delay(500);
        MC20_send_End_Mark();
        if(!MC20_wait_for_resp("SEND OK\r\n", DATA, DEFAULT_TIMEOUT * 10, DEFAULT_INTERCHAR_TIMEOUT * 10)) {
            return 0;
        }        
    }
    return len;
}

boolean GPSTracker::send(const char * str)
{
    if(!MC20_check_with_cmd("AT+CIPSEND\r\n",">",CMD)) {
        return false;
    }
    delay(500);
    MC20_send_cmd(str);
    delay(500);
    MC20_send_End_Mark();
    if(!MC20_wait_for_resp("SEND OK\r\n", DATA, DEFAULT_TIMEOUT * 10, DEFAULT_INTERCHAR_TIMEOUT * 10)) {
        return false;
    }        
    return true;
}


int GPSTracker::recv(char* buf, int len)
{
    MC20_clean_buffer(buf,len);
    MC20_read_buffer(buf,len);   //Ya he llamado a la funcion con la longitud del buffer - 1 y luego le estoy añadiendo el 0
    return strlen(buf);
}

// void GPSTracker::listen(void)
// {
//     serialMC20.listen();
// }

// bool GPSTracker::isListening(void)
// {
//     return serialMC20.isListening();
// }

uint32_t GPSTracker::str_to_ip(const char* str)
{
    uint32_t ip = 0;
    char* p = (char*)str;
    for(int i = 0; i < 4; i++) {
        ip |= atoi(p);
        p = strchr(p, '.');
        if (p == NULL) {
            break;
        }
        ip <<= 8;
        p++;
    }
    return ip;
}

char* GPSTracker::getIPAddress()
{
    //I have already a buffer with ip_string: snprintf(ip_string, sizeof(ip_string), "%d.%d.%d.%d", (_ip>>24)&0xff,(_ip>>16)&0xff,(_ip>>8)&0xff,_ip&0xff); 
    return ip_string;
}

unsigned long GPSTracker::getIPnumber()
{
    return _ip;
}
/* NOT USED bool GPSTracker::gethostbyname(const char* host, uint32_t* ip)
{
    uint32_t addr = str_to_ip(host);
    char buf[17];
    //snprintf(buf, sizeof(buf), "%d.%d.%d.%d", (addr>>24)&0xff, (addr>>16)&0xff, (addr>>8)&0xff, addr&0xff);
    if (strcmp(buf, host) == 0) {
        *ip = addr;
        return true;
    }
    return false;
}
*/