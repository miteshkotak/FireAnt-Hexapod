//=======================================================================================================
// Kurt's Ant test program.  Started off just doing servos connected to Orion, but added this and that...
//=======================================================================================================
//#define CUSTOMPINS

//#define DEBUG      //include debug serial outputs

#include <EEPROM.h>
#include <BMServo.h>
#include <BMSerial.h>
#include <BMPS2.h>
#include <SPI.h>
#include <Orion.h>

#define DEBUG
#define CALIBRATE
#define NORMAL

#define DEGREE 178//134
#define EEKEY 0x1234  //eeprom key to determine if arduino eeprom has valid data

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

#define TailYawPin  -9
#define TailPitchPin -10


#define DEADZONE 10

#define GREENLED A1
#define REDLED A0
//leg servo pin assignments
const int IKPins[] = {
  LFC,LFF,LFT,
  LMC,LMF,LMT,
  LRC,LRF,LRT,
  RFC,RFF,RFT,
  RMC,RMF,RMT,
  RRC,RRF,RRT,
  HeadPitchPin,HeadRollPin,HeadYawPin,PincerLPin,PincerRPin,
  TailYawPin, TailPitchPin};
#define CNTPINS (sizeof(IKPins)/sizeof(IKPins[0]))                   

unsigned char reverse[] = {
  0,1,0,
  0,1,0,
  0,1,0,
  1,0,1,
  1,0,1,
  1,0,1,
  0,0,0,0,1,
  0, 0};

const char* IKPinsNames[] = {
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

int aiPositions[CNTPINS];
boolean onoff = false;
byte iNextPin = 0xff;
int PinVal = 0;
int PincerPos = 0;

BMPS2 ps2x(6,100);

// Define the Tail servos which are run by the Arduino
BMServo TailYawServo(9);
BMServo TailPitchServo(10);


// foreword references
extern boolean DebugMonitor();
boolean g_fDebugOutput = false;

//=======================================================================================================
// Setup
//=======================================================================================================
void setup() {

  pinMode(GREENLED, OUTPUT);
  pinMode(REDLED, OUTPUT);
  digitalWrite(GREENLED, HIGH);
  // Lets enable buttons, 
  pinMode(2, INPUT);
  pinMode(4, INPUT);
  // IF both buttons are high neither is pushed at startup so init the Serial port
  if (!digitalRead(2) || !digitalRead(4)) {
    for (;;) {
      digitalWrite(REDLED, !digitalRead(REDLED));
      delay(250);
    }
  }  

  // Need to put after the test for buttons, so Orion can be updated
  Serial.begin(38400);

  // initialize Orion
  Orion.begin();

  InitArduinoServos();

  StopServos();
  delay(1000);  // give some times for the serial port to init...
  // Check to see if it looks like the servo offsets are set or not...
  int sOffset;
  fUseServoOffsets = false;
  for(int i = 0;i<CNTPINS;i++) {
    if (IKPins[i] >= 0) {
      sOffset = Orion.queryAOffset(IKPins[i]);
      if (abs(sOffset) > 10) {
        fUseServoOffsets = true;
        break;
      }
    }
  }
  delay(500);
  Serial.println("***Ant Test Program***");
  if (fUseServoOffsets)
    Serial.println("Servo Offsets Set");
  else
    Serial.println("Servo Offsets are not set");


  //Clears pressed/released states
  ps2x.newButtonState();
  delay(100);
  ps2x.newButtonState();

}



//=======================================================================================================
// Loop
//=======================================================================================================
void loop() {
  int ry;
  if(Orion.checkLipo()){
    StopServos();
    return; //Battery too low to do anything.
  }

  if (TerminalMonitor())
    return;

  TailPitchServo.update();
  TailYawServo.update();


  if (ps2x.buttonPressed(PSB_SELECT)) {
    PS2Test();
    while (ps2x.buttonPressed(PSB_SELECT)) {
      delay(50);
    }
    return;
  }

  if (/*ps2x.buttonPressed(PSB_L2) && */ps2x.buttonPressed(PSB_R3)) {
    Orion.resetRegs();
    Serial.println("Reset Registers");
    Serial.flush();
    asm volatile ("jmp 0x0000");  //reset
  }

  if (ps2x.buttonPressed(PSB_CROSS)) {
    PrintServoInfo();
  }

  if(ps2x.buttonPressed(PSB_START)){
    ToggleServosOnOrOff();
  }

  if(!onoff){
    digitalWrite(GREENLED,HIGH);

    // We are not in the on state, but lets use this state anyway to see when servos change...
    CheckForServosMoving();
    return;  
  }


  if(ps2x.buttonPressed(PSB_PAD_RIGHT)){
    iNextPin++;
    if (iNextPin >= CNTPINS)
      iNextPin = 0;

    WiggleServo(iNextPin);    
  }

  if(ps2x.buttonPressed(PSB_PAD_LEFT)){
    if (iNextPin)
      iNextPin--;
    else  
      iNextPin = CNTPINS -1;
    WiggleServo(iNextPin);  
  }

  // Now lets be able to move the servos some...
  ry = ps2x.analog(PSS_RY);
  if ((abs(ry) > DEADZONE) && (!Orion.queryMove())){  // allow some dead space and only if nothing moving...
    PinVal +=  ry/2;  // Allow the servo to move

    SetServoAngle(IKPins[iNextPin], PinVal, 40);
    Serial.print(IKPins[iNextPin], DEC);
    Serial.print(" : ");
    Serial.println(PinVal, DEC);
  }



  if(ps2x.buttonPressed(PSB_R1)){
    PincerPos -= DEGREE/2;
    MovePincers();

  }
  if(ps2x.buttonPressed(PSB_L1)){
    PincerPos += DEGREE/2;
    MovePincers();
  }
}


//=======================================================================================================
// SetServoAngle
//=======================================================================================================
void SetServoAngle(int iPin, int iVal, long lTime) {
  if (iPin >= 0) {
    Orion.setTime(lTime);
    Orion.setAngle(iPin, iVal);
    Orion.execute();
    delay(lTime);
  }
  else {
    if (iPin == TailYawPin) {
      TailYawServo.setAngle(iVal,lTime);
    }
    else if (iPin == TailPitchPin) {
      TailPitchServo.setAngle(iVal,lTime);
    }
    while (lTime > 0) {
      TailPitchServo.update();
      TailYawServo.update();
      delay(20);
      lTime -= 20;
    }
  }
}

//=======================================================================================================
// WiggleServo
//=======================================================================================================
void WiggleServo(int iServo) {
  int iPin = IKPins[iServo];
  Serial.print("Servo: ");
  Serial.print(iPin, DEC);
  Serial.print(" : ");
  Serial.println(IKPinsNames[iServo]);

  int sOffset = fUseServoOffsets? ServoOffsets[iServo] : 0;

  // Now lets wiggle that pin...
  SetServoAngle(iPin, sOffset + 100, 250);
  SetServoAngle(iPin, sOffset -100, 500);
  SetServoAngle(iPin, sOffset, 250);
  PinVal = sOffset;  // zero out the pin value...
}

//=======================================================================================================
//=======================================================================================================
void MovePincers(void) {
  Orion.setTime(250);
  Orion.setAngle(PincerLPin,PincerPos);
  Orion.setAngle(PincerRPin,PincerPos);
  Orion.execute();
  delay(250);
  Serial.print("P: ");
  Serial.print(PincerPos, DEC);
  Serial.print(" F: ");
  Serial.print(Orion.queryForce(PincerLPin), DEC);
  Serial.print(" ");
  Serial.println(Orion.queryForce(PincerRPin), DEC);

}

//=======================================================================================================
// Stop Servos
//=======================================================================================================
void StopServos(void) {
  for(int i = 0;i<CNTPINS;i++) {
    if (IKPins[i] >= 0) 
      Orion.stopPulse(IKPins[i]);
  }

  // Also stop Arduino Servos
  TailPitchServo.stopPulse();
  TailYawServo.stopPulse();

}  

//=======================================================================================================
// InitArduinoServos
//=======================================================================================================
void InitArduinoServos()
{
  TailPitchServo.begin();
  TailYawServo.begin();
  unsigned int key = (int)((EEPROM.read(510)<<8) | EEPROM.read(511));
  if(key==EEKEY){
    int offset=(int)((EEPROM.read((29*2)+0)<<8) | (EEPROM.read((29*2)+1)&0xFF));
    TailPitchServo.setAOffset(offset);
    offset=(int)((EEPROM.read((30*2)+0)<<8) | (EEPROM.read((30*2)+1)&0xFF));
    TailYawServo.setAOffset(offset);
    Serial.println("Arduino Servos Init");
  }
}


//=======================================================================================================
// QueryServoFBAngle
//=======================================================================================================
int QueryServoFBAngle(int iPin) {
  if (iPin >= 0)
    return Orion.queryFBAngle(iPin);

  else if (iPin == TailYawPin) {
    return TailYawServo.queryFBAngle();
  }
  else if (iPin == TailPitchPin) {
    return TailPitchServo.queryFBAngle();
  }
  return 0;
}    

//=======================================================================================================
// PrintServoInfo
//=======================================================================================================
void PrintServoInfo() {
  // Will print out information about each of the servos...
  int i;
  for (int j=0; j < CNTPINS; j++) {
    Serial.print(i = IKPins[j], DEC);
    Serial.print("(");
    Serial.print(IKPinsNames[j]);
    Serial.print(")");
    Serial.print(":");
    Serial.print(QueryServoFBAngle(i), DEC);
    if ( i >= 0) {
      Serial.print(" ");
      Serial.print(Orion.queryForce(i), DEC);
      Serial.print(" : ");
      Serial.print(Orion.queryAOffset(i), DEC);
      Serial.print("(" );
      Serial.print(Orion.queryServoMin(i), DEC);
      Serial.print(",");
      Serial.print(Orion.queryServoMax(i), DEC);
      Serial.print(")");
    } 
    else {
      // The only other valid data in the BMServo is the offset alue...
      Serial.print(" : ");
      if (i == TailYawPin) 
        Serial.print(TailYawServo.getAOffset(), DEC);
      else if (i == TailPitchPin)
        Serial.print(TailPitchServo.getAOffset(), DEC);
    } 
    if ((j % 3) == 2)
      Serial.println("");
    else  
      Serial.print("\t");
  }
}

//=======================================================================================================
//=======================================================================================================
void ToggleServosOnOrOff() {
  if(onoff){
    onoff=false;
    Orion.tone(NOTE_G4,100);
    StopServos();
  }
  else{
    onoff=true;

    Orion.setTime(0);
    Orion.setAngle(HeadPitchPin,Orion.queryFBAngle(HeadPitchPin));
    Orion.setAngle(HeadYawPin,Orion.queryFBAngle(HeadYawPin));
    Orion.setAngle(HeadRollPin,Orion.queryFBAngle(HeadRollPin));
    Orion.setAngle(PincerLPin,Orion.queryFBAngle(PincerLPin));
    Orion.setAngle(PincerRPin,Orion.queryFBAngle(PincerRPin));
    for(int i=0;i<CNTPINS;i++){
      if (IKPins[i] >= 0)
        Orion.setAngle(IKPins[i],Orion.queryFBAngle(IKPins[i]));
    }
    Orion.execute();

    int angle = TailPitchServo.queryFBAngle();
    TailPitchServo.setAngle(angle,20);
    angle = TailYawServo.queryFBAngle();
    TailYawServo.setAngle(angle,20);
    delay(100);

    Orion.setTime(1000);
    for(int i=0;i<CNTPINS;i++){
      if (IKPins[i] >= 0)
        Orion.setAngle(IKPins[i], fUseServoOffsets? ServoOffsets[i] : 0);    // Start all servos to our offsets
    }
    Orion.execute();
    TailPitchServo.setAngle(0,1000);
    TailYawServo.setAngle(0,1000);
    for(int i = 0;i<50;i++){
      TailPitchServo.update();
      TailYawServo.update();
      delay(20);
    }

    Orion.tone(NOTE_A5,100);
    Orion.tone(NOTE_B5,100);
    Orion.tone(NOTE_C6,100);
    ps2x.newButtonState();  //Clears pressed/released states

    //      Orion.EnableForce(PincerLPin, 1);
    //      Orion.EnableForce(PincerRPin, 1);
  }
}

//=======================================================================================================
//=======================================================================================================
void  CheckForServosMoving() {
#define MIN_REPORT_SERVO_MOVE 10
  int iPosition;
  for (int j=0; j < CNTPINS; j++) {
    iPosition = QueryServoFBAngle(IKPins[j]);
    if (abs(iPosition - aiPositions[j]) > MIN_REPORT_SERVO_MOVE) {
      aiPositions[j] = iPosition;
      Serial.print(IKPins[j], DEC);
      Serial.print("(");
      Serial.print(IKPinsNames[j]);
      Serial.print(")=");
      Serial.println(iPosition, DEC);
    }
  }
}



//=======================================================================================================
//=======================================================================================================
#define AVALUE_MASK 0x3fff 
void PS2Test(){
  int aiAValues[16];
  int iAValue;
  unsigned int wButtons;
  byte i;
  boolean fChanged;
  word wMask;

  Serial.println("PS2 Test");

  while (ps2x.buttonPressed(PSB_SELECT)) {
    delay(50);
  }

  ps2x.newButtonState();  //Clears pressed/released states
  Serial.println("Press Select again to exit");
  while (!ps2x.buttonPressed(PSB_SELECT)) {
    if (ps2x.isUpdated()) {
      fChanged = false;
      // the raw button data is not available, so build our own...
      word wNewButtons = 0;
      for (word wMask=0x8000; wMask; wMask >>=1) {
        if (ps2x.button(wMask))
          wNewButtons |= wMask;
      };
      if (wButtons != wNewButtons) {
        wButtons = wNewButtons;
        fChanged = true;
      }
      wMask = AVALUE_MASK;
      for(i=0; wMask; i++, wMask >>=1) {
        if(wMask & 1) {
          iAValue = ps2x.analog(i);
          if (iAValue != aiAValues[i]) {
            fChanged = true;
            aiAValues[i] = iAValue;
          }
        }
      }
      if (fChanged) {
        Serial.print(wButtons, HEX);
        wMask = AVALUE_MASK;
        for(i=0; wMask; i++, wMask >>=1) {
          if(wMask & 1) {
            Serial.print(" ");
            Serial.print(aiAValues[i], DEC);          
          }
        }
        Serial.println("");
      }
    }
  }   
  Serial.println("PS2 Test exited");
}

//==============================================================================
// TerminalMonitor - Simple background task checks to see if the user is asking
//    us to do anything, like update debug levels ore the like.
//==============================================================================
boolean g_fShowDebugPrompt = true;

boolean TerminalMonitor(void)
{
  byte szCmdLine[20];  // currently pretty simple command lines...
  byte ich;
  int ch;
  // See if we need to output a prompt.
  if (g_fShowDebugPrompt) {
    Serial.println(F("Fire Ant Servo Test Monitor"));
    Serial.println(F("D - Toggle debug on or off"));
    Serial.println(F("O - Toggle Servos on or Off"));
    Serial.println(F("P - Print Servo info"));
    Serial.println(F("<n> - Select servo"));
#ifdef OPT_DUMP_EEPROM
    Serial.println(F("E - Dump EEPROM"));
#endif
    // Let the Servo driver show it's own set of commands...
    g_fShowDebugPrompt = false;
  }


  // First check to see if there is any characters to process.
  if ((ich = Serial.available())) {
    ich = 0;
    // For now assume we receive a packet of data from serial monitor, as the user has
    // to click the send button...
    for (ich=0; ich < sizeof(szCmdLine); ich++) {
      ch = Serial.read();        // get the next character
      if ((ch == -1) || ((ch >= 10) && (ch <= 15)))
        break;
      szCmdLine[ich] = ch;
    }
    szCmdLine[ich] = '\0';    // go ahead and null terminate it...
    Serial.print(F("Serial Cmd Line:"));        
    Serial.write(szCmdLine, ich);
    Serial.println(F("<eol>"));

    // So see what are command is.
    if (ich == 0) {
      g_fShowDebugPrompt = true;
    } 
    else if ((ich == 1) && ((szCmdLine[0] == 'd') || (szCmdLine[0] == 'D'))) {
      g_fDebugOutput = !g_fDebugOutput;
      if (g_fDebugOutput) 
        Serial.println(F("Debug is on"));
      else
        Serial.println(F("Debug is off"));
    } 

    else if (((szCmdLine[0] == 'p') || (szCmdLine[0] == 'P'))) {
      PrintServoInfo();
    } 
    else if (((szCmdLine[0] == 'o') || (szCmdLine[0] == 'O'))) {
      ToggleServosOnOrOff();
    }
#ifdef OPT_DUMP_EEPROM
    else if (((szCmdLine[0] == 'e') || (szCmdLine[0] == 'E'))) {
      DumpEEPROMCmd(szCmdLine);
    } 
#endif

    return true;
  }
  return false;
}









