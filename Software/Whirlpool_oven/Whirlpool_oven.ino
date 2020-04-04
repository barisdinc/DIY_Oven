#include "Arduino.h"


#define SENSOR1 A0
#define SENSOR2 A1

#define I2CSDA  A4
#define I2CSCL  A5

#define ROLE6   2
#define ROLELER 3
#define ROLE4   4
#define ROLE1   5
#define ROLE3   6
#define ROLE2   7

#define V220    8 //INPUT

#define EEPROM_ENABLE 9


void setup() {
  pinMode (ROLELER, OUTPUT) ;
  TCCR2A = 0x23 ;
  TCCR2B = 0x09 ;
  OCR2A = 0x01 ;
  OCR2B = 0x00 ;
  TCNT2 = 0x00 ;


  
  pinMode(ROLE1,   OUTPUT);
  pinMode(ROLE2,   OUTPUT);
  pinMode(ROLE3,   OUTPUT);
  pinMode(ROLE4,   OUTPUT);
  pinMode(ROLE6,   OUTPUT);
 
  pinMode(EEPROM_ENABLE, OUTPUT);
  pinMode(V220,    INPUT);
  
}

void loop() {
  digitalWrite(ROLE2, HIGH);  
  delay(3000);               
  digitalWrite(ROLE2, LOW); 
  delay(1000);               
 
  }
