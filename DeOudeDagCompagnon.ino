/* 
 * Arduino code voor mijn project CMT Oude Dag Compagnon 
 * Bestaande uit:
 * Arduino Uno 
 * MPR121 + Conductive thread op electrodes 1, 4, 8 & 10
 * DFPlayer mini + speaker en SD kaart met 4 MP3 files
 */
 
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
#include "mpr121.h"
#include <Wire.h>

SoftwareSerial mySerial(10, 11);  // RX, TX
DFPlayerMini_Fast myMP3;          // Instantiate the DFPlayer

int irqpin = 2;                   // Digital 2
boolean touchStates[12];          //to keep track of the previous touch states

void setup() 
{
  mySerial.begin(9600);
  myMP3.begin(mySerial);
  myMP3.volume(20); // Volume at 77%
  
  Serial.begin(9600);
  pinMode(irqpin, INPUT);
  digitalWrite(irqpin, HIGH); //enable pullup resistor
  Wire.begin();
  mpr121_setup();
}

void loop() 
{
  readTouchInputs();
  //myMP3.wakeUp();
}

void readTouchInputs()
{
  if(!checkInterrupt()){

    //read the touch state from the MPR121
    Wire.requestFrom(0x5A,2); 

    byte LSB = Wire.read();
    byte MSB = Wire.read();

    uint16_t touched = ((MSB << 8) | LSB); //16bits that make up the touch states

    for (int i=0; i < 12; i++)
    {  // Check what electrodes were pressed
      if(touched & (1<<i))
      {
        if(touchStates[i] == 0)
        {
          //pin i was just touched
          Serial.print("pin ");
          Serial.print(i);
          Serial.println(" was just touched");

          // Switch statement for 4 paws telling the MP3 player to play 001.mp3 to 004.mp3 from the SD card
          switch (i) 
          {
            case 1: 
            Serial.println("paw 1 was just touched");
            myMP3.play(1);
            break;
            
            case 4: 
            Serial.println("paw 2 just touched");
            myMP3.play(2);
            break;
            
            case 8: 
            Serial.println("paw 3 was just touched");
            myMP3.play(3);
            break;
            
            case 10:
            Serial.println("paw 4 was just touched");
            myMP3.play(4);
            break;
          }
          
        }else if(touchStates[i] == 1)
        {
          //pin i is still being touched
        }  
        touchStates[i] = 1;      
      }else
      {
        if(touchStates[i] == 1)
        {
          Serial.print("pin ");
          Serial.print(i);
          Serial.println(" is no longer being touched");
          //pin i is no longer being touched
        }
        touchStates[i] = 0;
      }
    }
  }
}


void mpr121_setup(void)
{
  set_register(0x5A, ELE_CFG, 0x00); 

  // Section A - Controls filtering when data is > baseline.
  set_register(0x5A, MHD_R, 0x01);
  set_register(0x5A, NHD_R, 0x01);
  set_register(0x5A, NCL_R, 0x00);
  set_register(0x5A, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(0x5A, MHD_F, 0x01);
  set_register(0x5A, NHD_F, 0x01);
  set_register(0x5A, NCL_F, 0xFF);
  set_register(0x5A, FDL_F, 0x02);

  // Section C - Sets touch and release thresholds for each electrode
  set_register(0x5A, ELE0_T, TOU_THRESH);
  set_register(0x5A, ELE0_R, REL_THRESH);

  set_register(0x5A, ELE1_T, TOU_THRESH);
  set_register(0x5A, ELE1_R, REL_THRESH);

  set_register(0x5A, ELE2_T, TOU_THRESH);
  set_register(0x5A, ELE2_R, REL_THRESH);

  set_register(0x5A, ELE3_T, TOU_THRESH);
  set_register(0x5A, ELE3_R, REL_THRESH);

  set_register(0x5A, ELE4_T, TOU_THRESH);
  set_register(0x5A, ELE4_R, REL_THRESH);

  set_register(0x5A, ELE5_T, TOU_THRESH);
  set_register(0x5A, ELE5_R, REL_THRESH);

  set_register(0x5A, ELE6_T, TOU_THRESH);
  set_register(0x5A, ELE6_R, REL_THRESH);

  set_register(0x5A, ELE7_T, TOU_THRESH);
  set_register(0x5A, ELE7_R, REL_THRESH);

  set_register(0x5A, ELE8_T, TOU_THRESH);
  set_register(0x5A, ELE8_R, REL_THRESH);

  set_register(0x5A, ELE9_T, TOU_THRESH);
  set_register(0x5A, ELE9_R, REL_THRESH);

  set_register(0x5A, ELE10_T, TOU_THRESH);
  set_register(0x5A, ELE10_R, REL_THRESH);

  set_register(0x5A, ELE11_T, TOU_THRESH);
  set_register(0x5A, ELE11_R, REL_THRESH);

  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(0x5A, FIL_CFG, 0x04);

  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(0x5A, ELE_CFG, 0x0C);  // Enables all 12 Electrodes

  // Section F
  // Enable Auto Config and auto Reconfig
  /*set_register(0x5A, ATO_CFG0, 0x0B);
  set_register(0x5A, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(0x5A, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
  set_register(0x5A, ATO_CFGT, 0xB5);*/  // Target = 0.9*USL = 0xB5 @3.3V

  set_register(0x5A, ELE_CFG, 0x0C);
}


boolean checkInterrupt(void)
{
  return digitalRead(irqpin);
}


void set_register(int address, unsigned char r, unsigned char v)
{
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
}
