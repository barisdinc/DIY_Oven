#include "Arduino.h"

#define SENSOR1 A0 //Oven Temperature
#define SENSOR2 A1 //Water Tank Water Level
#define SENSOR3 A2 //Water Tank Temperature
#define I2CSDA  A4
#define I2CSCL  A5
#define ROLE6   2  //Back Fan
#define ROLELER 3
#define ROLE4   4  //Middle Heater ????
#define ROLE1   5  //Top Coolong Fan
#define ROLE3   6
#define ROLE2   7  //Bottom Heater
#define V220    8  //INPUT   
#define WTPUMP  9  //Water Tank Pump
#define WTHEAT  10 // Water Tank Heater

#define EEPROM_ENABLE 9

//avrdude -Cavrdude.conf -v -patmega328p -carduino -P/dev/ttyUSB0 -b57600 -D -Uflash:w:RGB.ino.hex:i 

String commandString = ""; 
//commandString.reserve(200);
char serial_char;
bool commandComplete = false;  // whether the command is complete

 char komut;
 char konum;
 
 void print_commands()
 {
  Serial.println("0-Genel");
  Serial.println("1-Ust Fan");
  Serial.println("2-Alt Izgara");
  Serial.println("3-orta Izgara");
  Serial.println("4-Ust Grill");
  Serial.println("6-Motor");
  Serial.println("8-Water Pump");
  Serial.println("9-Water Heat");
 }

void setup() {
  Serial.begin(9600);
  print_commands();
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
  pinMode(WTPUMP,   OUTPUT);
  pinMode(WTHEAT,   OUTPUT);
  pinMode(EEPROM_ENABLE, OUTPUT);
  pinMode(V220,    INPUT);

}
void loop() {

  if (commandComplete)
  {
    Serial.println(komut);
    Serial.println(konum);
    if ((komut=='0') and (konum=='0')) {Serial.println("pwm off"); TCCR2B = 0x08;  }//ROLE5
    if ((komut=='0') and (konum=='1')) {Serial.println("pwm on "); TCCR2B = 0x09;  }//ROLE5
    if ((komut=='1') and (konum=='0')) {Serial.println("fan off"); digitalWrite(ROLE1, LOW);}
    if ((komut=='1') and (konum=='1')) {Serial.println("fan on "); digitalWrite(ROLE1, HIGH);};
    if ((komut=='2') and (konum=='0')) digitalWrite(ROLE2, LOW);
    if ((komut=='2') and (konum=='1')) digitalWrite(ROLE2, HIGH);
    if ((komut=='3') and (konum=='0')) digitalWrite(ROLE3, LOW);
    if ((komut=='3') and (konum=='1')) digitalWrite(ROLE3, HIGH);
    if ((komut=='4') and (konum=='0')) digitalWrite(ROLE4, LOW);
    if ((komut=='4') and (konum=='1')) digitalWrite(ROLE4, HIGH);
    if ((komut=='6') and (konum=='0')) digitalWrite(ROLE6, LOW);
    if ((komut=='6') and (konum=='1')) digitalWrite(ROLE6, HIGH);
    if ((komut=='8') and (konum=='1')) digitalWrite(WTPUMP, HIGH);
    if ((komut=='8') and (konum=='0')) digitalWrite(WTPUMP, LOW);
    if ((komut=='9') and (konum=='1')) digitalWrite(WTHEAT, HIGH);
    if ((komut=='9') and (konum=='0')) digitalWrite(WTHEAT, LOW);
    if (!((komut <= 9) and (komut >=0))) print_commands();
    Serial.println(digitalRead(ROLE1));
    Serial.println(digitalRead(ROLE2));
    Serial.println(digitalRead(ROLE3));
    Serial.println(digitalRead(ROLE4));
    Serial.println(digitalRead(ROLE6));
    int analog1 = analogRead(SENSOR1);
    int analog2 = analogRead(SENSOR2);
    int analog3 = analogRead(SENSOR3);
    Serial.print("Oven Temp = ");
    Serial.println(analog1);
    Serial.print("Water Level = ");
    Serial.println(analog2);
    Serial.print("Water Temp  = ");
    Serial.println(analog3);
    commandComplete = false;
 } //if
 
 //} //while


  
  }


void serialEvent() {
  //Serialprint(".");
  cli();
  while (Serial.available()) {
    //char inChar = UDR0; //
    char inChar = (char)Serial.read();
    //Serial.print(inChar); //local echo
    commandString += inChar;
    if (inChar == '\n') {
      commandComplete = true;
      //Serial.println(commandString[0]);
      //Serial.println(commandString[1]);
      komut = commandString[0];
      konum = commandString[1];
      commandString="";
    }
  }
  sei();
}