//---------------------------------------------------------------------------------------------------
// Train data receiver using MIRF module
//
// zapmaker.org
//
// (c) zapmaker
// MIT License
//---------------------------------------------------------------------------------------------------
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define DATA_GYRO       0x10
#define DATA_SPEED      0x11
#define DATA_MOTOR      0x12

RF24 radio(8,7);//ce,cs
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

#define PAYLOAD_LEN  16
int curr, reportLastTime = 0;
byte dataIn[PAYLOAD_LEN];
void setup()
{
  Serial.begin(115200);

  radio.begin();
  radio.setRetries(15,15);
  radio.openReadingPipe(0,pipes[0]);
  radio.openWritingPipe(pipes[1]);
  radio.startListening();
  radio.printDetails();
  Serial.println("Listening..."); 
}

void loop()
{
  curr = millis();
  if ((curr - reportLastTime) > 2000) 
  {
    // do something periodically if needed
    reportLastTime = curr;
  }  
  
  if (haveData())
  {  
    if (dataIn[0] != 0)
    {
      //Serial.println("Got packet");
      
      int len = dataIn[1];
      if (len > PAYLOAD_LEN)
        len = PAYLOAD_LEN;

/*      
      for (int i = 0; i < len; i++)
      {
        Serial.print(dataIn[i], HEX);
        if (i < (len - 1))
          Serial.print(",");
      }
  
      Serial.print("  ");
*/      
      Serial.print(dataIn[2]);
      Serial.print(",");      

      switch (dataIn[2])
      {
        case DATA_GYRO:
        {
          Serial.print("G,");
          unsigned long tm = longFromBytes(&dataIn[3]);          
          
          int gx = intFromBytes(&dataIn[7]);
          int gy = intFromBytes(&dataIn[9]);
          int gz = intFromBytes(&dataIn[11]);
      
          Serial.print(tm);
          Serial.print(",");           
          Serial.print(gx);
          Serial.print(",");    
          Serial.print(gy);
          Serial.print(",");    
          Serial.print(gz);
          break;
        }
        case DATA_SPEED:
        {
          Serial.print("S,");
          unsigned long tm = longFromBytes(&dataIn[3]);
      
          unsigned long wrc = longFromBytes(&dataIn[7]);
          int itd = intFromBytes(&dataIn[11]);
          int ispd = intFromBytes(&dataIn[13]);
      
          Serial.print(tm);
          Serial.print(",");           
          Serial.print(wrc);
          Serial.print(",");    
          Serial.print(itd);
          Serial.print(",");    
          Serial.print(ispd);
          break;
        }
        case DATA_MOTOR:
        {
          Serial.print("M,");
          unsigned long tm = longFromBytes(&dataIn[3]);
      
          int mpwm = intFromBytes(&dataIn[7]);
      
          Serial.print(tm);
          Serial.print(",");           
          Serial.print(mpwm);
          break;
        }
        default:
          Serial.print("U");
          break;
      }
      Serial.println();
    }
  }
}

bool haveData()
{
  // if there is data ready
  if ( radio.available() )
  {
    radio.read( dataIn, PAYLOAD_LEN );
    return true;
  }
  return false;
}

unsigned long longFromBytes(byte *ptr)
{
  return (((unsigned long)ptr[0] << 24) & 0xFF00000) 
                | (((unsigned long)ptr[1] << 16) & 0xFF0000) 
                | (((unsigned long)ptr[2] << 8) & 0xFF00) 
                | ((unsigned long)ptr[3] & 0xFF);
       
}

int intFromBytes(byte *ptr)
{
  return (((int)ptr[0] << 8) & 0xFF00) | ((int)ptr[1] & 0xFF);
}
