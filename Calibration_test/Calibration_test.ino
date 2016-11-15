#include <Orion.h>
#include <SPI.h>

const int IN_SERVO_PIN[18] = {23,22,21,20,19,18,17,16,0,15,14,13,12,11,10,9,8,1};
const int Aoffsets[18]={0,0,-500,372,0,-500,0,0,-500,0,0,-500,0,0,-500,0,0,-500};


 
void setup()
{
  Serial.begin( 9600 );
  Serial.print( "Calibrated Test ...\n" );

  Orion.begin(); 
  Orion.setSpeed(2000);
  Orion.tone(NOTE_C6,100); 

    Initpostion();

 }



void loop() {
  // put your main code here, to run repeatedly:





}
