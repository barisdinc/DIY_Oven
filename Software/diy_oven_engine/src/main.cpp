#include "Arduino.h"

#define SENS_OTMP A0 //Oven Temperature
#define SENS_WLVL A1 //Water Tank Water Level
#define SENS_WTMP A2 //Water Tank Temperature
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

 char cmd;
 char state;
 
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
  pinMode(SENS_OTMP, INPUT);
  pinMode(SENS_WLVL, INPUT);
  pinMode(SENS_WTMP, INPUT);

}
void loop() {

  if (commandComplete)
  {
    Serial.println(cmd);
    Serial.println(state);
    if ((cmd=='0') and (state=='0')) {Serial.println("pwm off"); TCCR2B = 0x08;  }//ROLE5
    if ((cmd=='0') and (state=='1')) {Serial.println("pwm on "); TCCR2B = 0x09;  }//ROLE5
    if ((cmd=='1') and (state=='0')) {Serial.println("fan off"); digitalWrite(ROLE1, LOW);}
    if ((cmd=='1') and (state=='1')) {Serial.println("fan on "); digitalWrite(ROLE1, HIGH);};
    if ((cmd=='2') and (state=='0')) digitalWrite(ROLE2, LOW);
    if ((cmd=='2') and (state=='1')) digitalWrite(ROLE2, HIGH);
    if ((cmd=='3') and (state=='0')) digitalWrite(ROLE3, LOW);
    if ((cmd=='3') and (state=='1')) digitalWrite(ROLE3, HIGH);
    if ((cmd=='4') and (state=='0')) digitalWrite(ROLE4, LOW);
    if ((cmd=='4') and (state=='1')) digitalWrite(ROLE4, HIGH);
    if ((cmd=='6') and (state=='0')) digitalWrite(ROLE6, LOW);
    if ((cmd=='6') and (state=='1')) digitalWrite(ROLE6, HIGH);
    if ((cmd=='8') and (state=='1')) digitalWrite(WTPUMP, HIGH);
    if ((cmd=='8') and (state=='0')) digitalWrite(WTPUMP, LOW);
    if ((cmd=='9') and (state=='1')) digitalWrite(WTHEAT, HIGH);
    if ((cmd=='9') and (state=='0')) digitalWrite(WTHEAT, LOW);
    if (!((cmd <= 9) and (cmd >=0))) print_commands();
    Serial.println(digitalRead(ROLE1));
    Serial.println(digitalRead(ROLE2));
    Serial.println(digitalRead(ROLE3));
    Serial.println(digitalRead(ROLE4));
    Serial.println(digitalRead(ROLE6));
    analogRead(SENS_OTMP); int ovent_temp  = analogRead(SENS_OTMP);
    analogRead(SENS_WLVL); int water_level = analogRead(SENS_WLVL);
    analogRead(SENS_WTMP); int water_temp  = analogRead(SENS_WTMP);
    Serial.print("Oven Temp = ");
    Serial.println(ovent_temp);
    Serial.print("Water Level = ");
    Serial.println(water_level);
    Serial.print("Water Temp  = ");
    Serial.println(water_temp);
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
      cmd = commandString[0];
      state = commandString[1];
      commandString="";
    }
  }
  sei();
}