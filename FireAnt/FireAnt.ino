//Version 1.0.0

#include <EEPROM.h>

#include <BMSerial.h>
#include <BMPS2.h>
#include <SPI.h>
#include <Orion.h>

//#define CUSTOMPINS

//#define DEBUG      //include debug serial outputs
#define CALIBRATE  //include calibration subroutines
#define PS2        //include PS2 control subroutines
//#define DEMO     //include demo subroutines

//Maximum body movement offsets
#define MAXX 625
#define MAXY 2000
#define MAXZ 550
#define MAXR 175
#define MAXBODYYAW 300
#define MAXBODYROLL 300
#define MAXBODYPITCH 300

//Servo pin numbers
  #ifdef CUSTOMPINS
#define LFC 21
#define LFF 22 
#define LFT 23
#define LMC 20
#define LMF 19
#define LMT 18
#define LRC 17
#define LRF 16
#define LRT 0
#define RFC 13
#define RFF 14
#define RFT 15
#define RMC 12
#define RMF 11
#define RMT 10
#define RRC 9
#define RRF 8
#define RRT 1
#define HeadPitchPin 3
#define HeadRollPin 7
#define HeadYawPin 6
#define PincerLPin 5
#define PincerRPin 4
  #else
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
  #endif

//leg dimensions(1/10 mm units)
const unsigned int IKLengths[24] = {525,802,1283,0,
                                    525,802,1283,0,
                                    525,802,1283,0,
                                    525,802,1283,0,
                                    525,802,1283,0,
                                    525,802,1283,0};

//Robot Initial Positions(1/10 mm units)
const int DefBodyPosX[6] = {706,706,706,-706,-706,-706};
const int DefBodyPosY[6] = {-370,-370,-370,-370,-370,-370};
const int DefBodyPosZ[6] = {841,0,-841,841,0,-841};
const int DefLegPosX[6] = {1083,1250,1083,-1083,-1250,-1083};
const int DefLegPosY[6] = {0,0,0,0,0,0};
const int DefLegPosZ[6] = {625,0,-625,625,0,-625};
const int DefAntPos[7] = {0,0,0,0,0,0,0};

//body rotation offsets: center,neck and tail
const int DefBodyRotOffset[3] = {0,-1430,1160}; 

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

