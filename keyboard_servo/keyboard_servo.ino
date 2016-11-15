
#include <EEPROM.h>
#include <BMServo.h>
#include <BMSerial.h>
#include <BMPS2.h>
#include <SPI.h>
#include <Orion.h>
#include<Servo.h>
#define DEBUG
#define CALIBRATE
#define NORMAL

#define DEGREE 178//134
#define EEKEY 0x1234  //eeprom key to determine if arduino eeprom has valid data

//Servo pin numbers
 

#define LFC 23
#define LFF 22 
#define LFT 21
#define LMC 20
#define LMF 19
#define LMT 18
#define LRC 17
#define LRF 16
#define LRT 0
#define RFC 15
#define RFF 14
#define RFT 13
#define RMC 12
#define RMF 11
#define RMT 10
#define RRC 9
#define RRF 8
#define RRT 1
#define HeadRollPin 7
#define HeadYawPin 6
#define HeadPitchPin 5
#define PincerLPin 4
#define PincerRPin 3


#define TailYawPin  -9
#define TailPitchPin -10

//leg servo pin assignments
const unsigned char servoPins[] = { LFC, LFF, LFT,   LMC, LMF, LMT,   LRC, LRF, LRT, 
                                    RFC, RFF, RFT,   RMC, RMF, RMT,   RRC, RRF, RRT,
                                    HeadRollPin, HeadYawPin, HeadPitchPin, PincerLPin, PincerRPin ,TailYawPin, TailPitchPin}; 
const unsigned int NUM_SERVOS = sizeof(servoPins)/sizeof(unsigned char); 


const char* servoPinsNames[] = {
  "LFC","LFF","LFT",
  "LMC","LMF","LMT",
  "LRC","LRF","LRT",
  "RFC","RFF","RFT",
  "RMC","RMF","RMT",
  "RRC","RRF","RRT",
  "Head Pitch(UD)","Head Roll (ROT)","Head Yaw (LR)","L Pincer", "R Pincer", 
  "Tail Yaw(LR)", "Tail Pitch(UD"};


void setup() {

    Serial.begin(9600);

  // initialize Orion
  Orion.begin();
  Orion.tone(NOTE_C6,100);
  Orion.tone(NOTE_B6,100);
  Orion.setTime( 1000 );
  Orion.setAngle(TailYawPin, 0 );
   

}

void loop() {
 /*int a= Serial.read();
  if (a > 0)
    
   {
   
   switch (a)
   {
       
   case 49:
             for (int i=0;i <= 180;i++)
            {
            myservo.write(i);              // tell servo to go to position in variable 'pos'
            delay(100000);
            }
    }
   }*/
}
