//=======================================================================================================
// Mitesh's Ant test program. 
//=======================================================================================================
//#define CUSTOMPINS

//#define DEBUG      //include debug serial outputs

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

//Maximum body movement offsets
#define MAXX 625
#define MAXY 2000
#define MAXZ 550
#define MAXR 175
#define MAXBODYYAW 300
#define MAXBODYROLL 300
#define MAXBODYPITCH 300



#define DEADZONE 10

#define GREENLED A1
#define REDLED A0

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

boolean fUseServoOffsets;
const int ServoOffsets[] = {
  0, 900,0,
  0, 900,0,
  0, 900,0,
  0, 900,0,
  0, 900,0,
  0, 900,0,
  0,0,0,0,0,
  0, 0};

 
//Servo angle offsets(1/10 degree units)
const int ServoAOffsets[29] = {0,-900,-900,0,
                               0,-900,-900,0,
                               0,-900,-900,0,
                               0,-900,-900,0,
                               0,-900,-900,0,
                               0,-900,-900,0,
                               0,0,0,0,0};
 

//Servo normal/reverse flags
const unsigned char reverse[29] = {0,1,0,0,
                                   0,1,0,0,
                                   0,1,0,0,
                                   1,0,1,0,
                                   1,0,1,0,
                                   1,0,1,0,
                                   0,0,0,0,1};

//leg dimensions(1/10 mm units)
const unsigned int IKLengths[24] = {525,802,1283,0,
                                    525,802,1283,0,
                                    525,802,1283,0,
                                    525,802,1283,0,
                                    525,802,1283,0,
                                    525,802,1283,0};

//leg servo pin assignments
const int IKPins[29] = {LFC,LFF,LFT,-1,
                        LMC,LMF,LMT,-1,
                        LRC,LRF,LRT,-1,
                        RFC,RFF,RFT,-1,
                        RMC,RMF,RMT,-1,
                        RRC,RRF,RRT,-1,
                        HeadPitchPin,HeadRollPin,HeadYawPin,PincerLPin,PincerRPin};

int aiPositions[NUM_SERVOS];
boolean onoff = false;
byte iNextPin = 0xff;
int PinVal = 0;
int PincerPos = 0;

BMServo TailYawServo(9);
BMServo TailPitchServo(10);


void setup() {
  // put your setup code here, to run once:

 pinMode(GREENLED, OUTPUT);
  pinMode(REDLED, OUTPUT);
  digitalWrite(GREENLED, HIGH);

  // Need to put after the test for buttons, so Orion can be updated
  Serial.begin(9600);

  // initialize Orion
  Orion.begin();
  //Orion.tone(NOTE_C6,100);
  //Orion.setTime( 5000 ); 
  //initangles();
 //PrintServoInfo();
}




 /*
      delay(5000);
   
   Orion.tone(NOTE_B6,100); 
   Orion.setTime( 3000 );
   Orion.setAngle( LFT,-850);
   Orion.setAngle( LFF,-300);
   
   Orion.setAngle( LMT,-850);
   Orion.setAngle( LMF,-300);
   
   Orion.setAngle( LRT,-850);
   Orion.setAngle( LRF,-300);
   
   Orion.setAngle( RFT,-850);
   Orion.setAngle( RFF,-300);
   
   Orion.setAngle( RMT,-850);
   Orion.setAngle( RMF,-300);
   
   Orion.setAngle( RRT,-850);
   Orion.setAngle( RRF,-300);
   //Orion.setAngle( LFT,600);
   Orion.execute();
    delay(4000); 
    Orion.tone(NOTE_C6,100); 
    Orion.setBalanceMode(0x2);
  Orion.setBalanceTransRate(50);
  Orion.setBalanceAngleRate(200);
  Orion.execute();  
  delay(4000);
  while(1){
  Orion.setTime(1000);
   Orion.setAngle( LFF,-50);
   Orion.setAngle( LRF,-50);
   Orion.setAngle( RMF,-50); 

   Orion.setAngle( LFC,100);
   Orion.setAngle( LRC,50);
   Orion.setAngle( RMC,100); 
  Orion.execute();
delay(2000);
  Orion.setTime(500);
   Orion.setAngle( LFF,-300);
   Orion.setAngle( LRF,-300);
   Orion.setAngle( RMF,-300);
  Orion.execute(); 
  delay(2000);
   Orion.setAngle( LFC,-100);
   Orion.setAngle( LRC,-100);
   Orion.setAngle( RMC,-100);  
  Orion.execute();
   delay(2000);
 //  Serial.print(Orion.queryFBAngle (LFT)); 
   //Serial.print("\n");
   //delay(5000);

  
   //Orion.setTime( 1000 );
  // Orion.setAngle( LFT,-600);
  // Orion.execute();
   
   //Serial.print(Orion.queryFBAngle (LFT));
  // Serial.print("\n");
   //delay(5000);  
    
    
    
  // put your main code here, to run repeatedly:
delay(4000);
  Orion.setTime(1000);
   Orion.setAngle( RFF,-50);
   Orion.setAngle( RRF,-50);
   Orion.setAngle( LMF,-50); 

   Orion.setAngle( RFC,100);
   Orion.setAngle( RRC,100);
   Orion.setAngle( LMC,50); 
  Orion.execute();
delay(2000);
  Orion.setTime(500);
   Orion.setAngle( RFF,-200);
   Orion.setAngle( RRF,-200);
   Orion.setAngle( LMF,-200);
  Orion.execute(); 
  delay(2000);
   Orion.setAngle( RFC,-100);
   Orion.setAngle( RRC,-100);
   Orion.setAngle( LMC,-100);  
  Orion.execute();
  }  







 */


