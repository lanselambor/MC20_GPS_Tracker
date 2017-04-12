/*
 * MC20_BT.cpp
 * A library for SeeedStudio GPS Tracker BT 
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

#include "MC20_GNSS.h"


bool GNSS::initialize()
{
    return true;
}

bool GNSS::open_GNSS(void)
{
  bool ret = true;

  if(!MC20_check_with_cmd("AT+QGNSSC?\n\r", "+QGNSSC: 1", CMD, 2, 2000)){
      ret = MC20_check_with_cmd("AT+QGNSSC=1\n\r", "OK", CMD, 2, 2000);  
  }

  // if(!MC20_check_with_cmd("AT+QIFGCNT=2\n\r", "OK", CMD, 2, 2000)){
  //   return false;
  // }

  // if(!MC20_check_with_cmd("AT+QGNSSC?\n\r", "+QGNSSC: 1", CMD, 2, 2000)){
  //     ret = MC20_check_with_cmd("AT+QGNSSC=1\n\r", "OK", CMD, 2, 2000);  
  // }

  // if(!MC20_check_with_cmd("AT+QGNSSC?\n\r", "+QGNSSC: 1", CMD, 2, 2000)){
  //     ret = MC20_check_with_cmd("AT+QGNSSC=1\n\r", "OK", CMD, 2, 2000);  
  // }

  return ret;
}

bool GNSS::getCoordinate(void)
{
    
    int i = 0;
    int j = 0;
    char *p = NULL;
    char buffer[512];
    char strLine[128];
    char *header = "$GNGGA,";

    p = &header[0];

    MC20_clean_buffer(buffer, 512);
    MC20_send_cmd("AT+QGNSSRD?\n\r");
    MC20_read_buffer(buffer, 512, 2);
    // SerialUSB.println(buffer);

    while(buffer[i] != '\0'){
        if(buffer[i] ==  *(p+j)){
            j++;
            // SerialUSB.print(i);
            // SerialUSB.println(buffer[i]);
            if(j >= 7) {
                p = &buffer[i];
                i = 0;
                while(*(p++) != '\n'){
                    // SerialUSB.write(*p);
                    strLine[i++] = *p;
                }
                strLine[i] = '\0';
                //SerialUSB.println(strLine);  // 093359.000,2235.0189,N,11357.9816,E,2,17,0.80,35.6,M,-2.5,M,,*51
                p = strtok(strLine, ",");
                p = strtok(NULL, ",");
                longitude = strtod(p, NULL);
                longitude /= 100.0;
                p = strtok(NULL, ",");
                p = strtok(NULL, ",");
                latitude = strtod(p, NULL);
                latitude /= 100.0;
                break;
            }
        } else {
            j = 0;
        }
        i++;
    }

    return true;
}

bool GNSS::dataFlowMode(void)
{
    // Make sure that "#define UART_DEBUG" is uncomment.
    return MC20_check_with_cmd("AT+QGNSSRD?\n\r", "OK", CMD);   
}

