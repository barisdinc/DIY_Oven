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
  Serial.begin(9600);
  pinMode (ROLELER, OUTPUT) ;
  TCCR2A = 0x23 ;
  TCCR2B = 0x08; // TCCR2B = 0x09 ;
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
 //digitalWrite(ROLE1, HIGH);
 //delay(1000);
 //TCCR2B = 0x09;  //ROLE5
 //delay(1000);
 //digitalWrite(ROLE4, HIGH);
 //delay(1000);
 //digitalWrite(ROLE6, HIGH);
 //TCCR2B = 0x08;  //ROLE5
 //delay(1000);

String serial_data;
char serial_char;
while (Serial.available() > 0 ) {
 int komut = Serial.parseInt(); 
 int konum = Serial.parseInt();
 if (Serial.read() == '\n') {
  Serial.println("0-Genel");
  Serial.println("1-Ust Fan");
  Serial.println("2-Alt Izgara");
  Serial.println("3-orta Izgara");
  Serial.println("4-Ust Grill");
  Serial.println("6-Motor");

  if ((komut==0) and (konum==0)) TCCR2B = 0x08;  //ROLE5
  if ((komut==0) and (konum==1)) TCCR2B = 0x09;  //ROLE5
  if ((komut==1) and (konum==0)) digitalWrite(ROLE1, LOW);
  if ((komut==1) and (konum==1)) digitalWrite(ROLE1, HIGH);
  if ((komut==2) and (konum==0)) digitalWrite(ROLE2, LOW);
  if ((komut==2) and (konum==1)) digitalWrite(ROLE2, HIGH);
  if ((komut==3) and (konum==0)) digitalWrite(ROLE3, LOW);
  if ((komut==3) and (konum==1)) digitalWrite(ROLE3, HIGH);
  if ((komut==4) and (konum==0)) digitalWrite(ROLE4, LOW);
  if ((komut==4) and (konum==1)) digitalWrite(ROLE4, HIGH);
  if ((komut==6) and (konum==0)) digitalWrite(ROLE6, LOW);
  if ((komut==6) and (konum==1)) digitalWrite(ROLE6, HIGH);
  Serial.println(digitalRead(ROLE1));
  Serial.println(digitalRead(ROLE2));
  Serial.println(digitalRead(ROLE3));
  Serial.println(digitalRead(ROLE4));
  Serial.println(digitalRead(ROLE6));
  
 } //if
 
 } //while


  
  }