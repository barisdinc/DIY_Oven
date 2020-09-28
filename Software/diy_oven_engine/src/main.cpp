#include "Arduino.h"
#include <PID_v1.h>         // P.I.D. temperature control library

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

const int maxTemp = 1600;              // Maximum temperature (degrees).  If reached, will shut down everything
const int numZones = 3;                // Number of heating element + thermostat zones (max of 3)
const int pidCycle = 2500;             // Time for a complete PID on/off cycle for the heating elements (ms)
double pidInput[numZones];             // Input array for PID loop (actual temp reading from thermocouple).  Don't change.
double pidOutput[numZones];            // Output array for PID loop (relay for heater).  Don't change.
double pidSetPoint[numZones];          // Setpoint array for PID loop (temp you are trying to reach).  Don't change.
PID pidCont[numZones] ;//= {PID(&pidInput[0], &pidOutput[0], &pidSetPoint[0], 800, 47.37, 4.93, DIRECT)};  // PID controller array for each zone.  Set arguments 4/5/6 to the Kp, Ki, Kd values after tuning.
const long saveCycle = 15000;          // How often to save current temp / setpoint (ms) 
const int tempOffset[numZones] = {0};  // Array to add a temp offset for each zone (degrees).  Use if you have a cold zone in your kiln or if your thermocouple reading is off.  This gets added to the setpoint.
const int tempRange = 2;               // This is how close the temp reading needs to be to the set point to shift to the hold phase (degrees).  Set to zero or a positive integer.
const char tempScale = 'C';            // Temperature scale.  F = Fahrenheit.  C = Celsius

const int heaterPin[numZones] = {9};            // Pins connected to relays for heating elements.  This is an array for each output pin (zone).

double calcSetPoint;        // Calculated set point (degrees)
unsigned long holdStart;    // Exact time the hold phase of the segment started (ms).  Based on millis().
int i = 0;                  // Simple loop counter
int lastSeg = 0;            // Last segment number in firing schedule
int lastTemp;               // Last setpoint temperature (degrees)
unsigned long lcdStart;     // Exact time you refreshed the lcd screen (ms).  Based on millis().
int optionNum = 1;          // Option selected from screen #3
unsigned long pidStart;     // Exact time you started the new PID cycle (ms).  Based on millis().
double rampHours;           // Time it has spent in ramp (hours)
unsigned long rampStart;    // Exact time the ramp phase of the segment started (ms).  Based on millis().
unsigned long saveStart;    // Exact time you saved the temps (ms).  Based on millis().


//sil
boolean schedOK = false;    // Is the schedule you loaded OK?
unsigned long schedStart;   // Exact time you started running the schedule (ms).  Based on millis().
int segHold[20];            // Hold time for each segment (min).  This starts after it reaches target temp.
int segNum = 0;             // Current segment number running in firing schedule.  0 means a schedule hasn't been selected yet.
boolean segPhase = 0;       // Current segment phase.  0 = ramp.  1 = hold.
int segRamp[20];            // Rate of temp change for each segment (deg/hr).
int segTemp[20];            // Target temp for each segment (degrees).


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

  // Open firing shedule # 1
  openSched();
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





  //******************************
  // Shutdown if too hot
  for (i = 0; i < numZones; i++) {
    if (pidInput[i] >= maxTemp) {
      lcd.clear();
      lcd.print(F("       ERROR:"));
      lcd.setCursor(2, 2);
      lcd.print(F("Max temp reached"));
      lcd.setCursor(0, 3);
      lcd.print(F("System was shut down"));    
      shutDown();
    }
  }

  //******************************
  // Select a firing schedule
  if (segNum == 0) {
    
    // Up arrow button
    if (digitalRead(upPin) == LOW && schedNum > 1) {
      schedNum = schedNum - 1;
      openSched();
      btnBounce(upPin);
    }
    
    // Down arrow button
    if (digitalRead(downPin) == LOW) {
      schedNum = schedNum + 1;
      openSched();
      btnBounce(downPin);          
    }
    
    // Select / Start button
    if (digitalRead(selectPin) == LOW && schedOK == true) {
      setupPIDs();
      segNum = 1;
      lcdStart = millis();
      pidStart = millis();
      rampStart = millis();
      schedStart = millis();
      updateLCD();
      btnBounce(selectPin);      
    }
  }

  //******************************
  // Running the firing schedule
  if (segNum >= 1) {

    // Up arrow button
    if (digitalRead(upPin) == LOW) {
      if (screenNum == 2 || (screenNum == 3 && optionNum == 1)) {
        screenNum = screenNum - 1;
      }
      else if (screenNum == 3 && optionNum >= 2) {
        optionNum = optionNum - 1;
      }
      updateLCD();
      btnBounce(upPin); 
    }
    
    // Down arrow button
    if (digitalRead(downPin) == LOW) {
      if (screenNum <= 2) {
        screenNum = screenNum + 1;
      }
      else if (screenNum == 3 && optionNum <= 2) {
        optionNum = optionNum + 1;
      }
      updateLCD();
      btnBounce(downPin); 
    }

    // Select / Start button
    if (digitalRead(selectPin) == LOW && screenNum == 3) {
      if (optionNum == 1) {  // Add 5 min
        segHold[segNum - 1] = segHold[segNum - 1] + 5;
        optionNum = 1;
        screenNum = 2;
      }

      if (optionNum == 2) {  // Add 5 deg
        segTemp[segNum - 1] = segTemp[segNum - 1] + 5;
        optionNum = 1;
        screenNum = 1;
      }

      if (optionNum == 3) {  // Goto next segment
        segNum = segNum + 1;
        optionNum = 1;
        screenNum = 2;
      }

      updateLCD();
      btnBounce(selectPin);           
    }
 
    // Update PID's / turn on heaters / update segment info
    if (screenNum < 4) {
      if (millis() - pidStart >= pidCycle) {
        pidStart = millis();
        updatePIDs();
      }
      htrControl();
      updateSeg();
    }

    // Refresh the LCD
    if (millis() - lcdStart >= lcdRefresh) {
      updateLCD();
      lcdStart = millis();
    }

    // Save the temps to a file on SD card
    if (millis() - saveStart >= saveCycle && pidInput[0] > 0 && screenNum < 4) {
      saveFile = SD.open("temps.txt", FILE_WRITE);
        saveFile.print((millis() - schedStart) / 60000.0); // Save in minutes
        for (i = 0; i < numZones; i++) {
          saveFile.print(",");
          saveFile.print(pidInput[i]);
        }
        saveFile.print(",");
        saveFile.println(pidSetPoint[0]);
      saveFile.close();

      saveStart = millis();
    }  
    
  }  
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


//******************************************************************************************************************************
//  OPENSCHED: OPEN AND LOAD A FIRING SCHEDULE FILE / DISPLAY ON SCREEN
//******************************************************************************************************************************
void openSched() {

  // Setup all variables
  int col = 1;          // Column number (of text file).  First column is one.
  int row = 1;          // Row number (of text file).  First row is one.
  char tempChar;        // Temporary character holder (read one at a time from file)
  char tempLine[21];    // Temporary character array holder
  int tempLoc = 0;      // Current location of next character to place in tempLine array
  char schedDesc2[21];  // Schedule description #2 (second line of text file)
  char schedDesc3[21];  // Schedule description #3 (third line of text file)  

  // Clear the arrays
  memset(schedDesc1, 0, sizeof(schedDesc1));
  memset(segRamp, 0, sizeof(segRamp));
  memset(segTemp, 0, sizeof(segTemp)); 
  memset(segHold, 0, sizeof(segHold));
  
  // Make sure you can open the file
  sprintf(tempLine, "%d.txt", schedNum);
  File myFile = SD.open(tempLine, FILE_READ);
  
  if (myFile == false) {
    lcd.clear();
    lcd.print(F("SELECT SCHEDULE: "));
    lcd.print(schedNum);
    lcd.setCursor(0, 2);
    lcd.print(F("Can't find/open file"));
    schedOK = false;
    return;
  }
 
   // Load the data
  while (myFile.available() > 0) {

    // Read a single character
    tempChar = myFile.read();

    if (tempChar == 13) {       // Carriage return: Read another char (it is always a line feed / 10).  Add null to end.
      myFile.read();
      tempLine[tempLoc] = '\0';
    }
    else if (tempChar == 44) {  // Comma: Add null to end.
      tempLine[tempLoc] = '\0';      
    }
    else if (tempLoc <= 19) {   // Add it to the temp line array
      tempLine[tempLoc] = tempChar;
      tempLoc = tempLoc + 1; 
    }

    if (row == 1 && tempChar == 13) {
      memcpy(schedDesc1, tempLine, 21);
    }
    else if (row == 2 && tempChar == 13) {
      memcpy(schedDesc2, tempLine, 21);      
    }
    else if (row == 3 && tempChar == 13) {
      memcpy(schedDesc3, tempLine, 21);      
    }
    else if (row >= 4 && col == 1 && tempChar == 44) {
      segRamp[row - 4] = atoi(tempLine);
    }
    else if (row >= 4 && col == 2 && tempChar == 44) {
      segTemp[row - 4] = atoi(tempLine);
    }
    else if ((row >= 4 && col == 3 && tempChar == 13) || myFile.available() == 0) {
      segHold[row - 4] = atoi(tempLine);
    }

    if (tempChar == 13) {  // End of line.  Reset everything and goto next line
      memset(tempLine, 0, 21);
      tempLoc = 0;
      row = row + 1;
      col = 1;
    }

    if (tempChar == 44) {  // Comma.  Reset everything and goto 1st column
      memset(tempLine, 0, 21);
      tempLoc = 0;
      col = col + 1;
    }
    
  }  // end of while(myFile.available ...

  // Close the file
  myFile.close();

  // Set some variables
  lastSeg = row - 3;
  schedOK = true;

  // Fix Ramp values so it will show the correct sign (+/-).  This will help to determine when to start hold.
  for (i = 0; i < lastSeg; i++) {
    segRamp[i] = abs(segRamp[i]);
    if (i >= 1) {
      if (segTemp[i] < segTemp[i - 1]) {
        segRamp[i] = -segRamp[i];
      }
    }
  } 

  // Display on the screen
  lcd.clear();
  lcd.print(F("SELECT SCHEDULE: "));
  lcd.print(schedNum);
  lcd.setCursor(0, 1);
  lcd.print(schedDesc1);
  lcd.setCursor(0, 2);
  lcd.print(schedDesc2);    
  lcd.setCursor(0, 3);
  lcd.print(schedDesc3);

  // Cut down schedule description so it shows better on other screen when running
  schedDesc1[14 - intLength(schedNum)] = '\0';

}

//******************************************************************************************************************************
//  HTRCONTROL: TURN HEATERS ON OR OFF
//******************************************************************************************************************************
void htrControl() {

  // Loop thru all zones
  for (i = 0; i < numZones; i++) {
    if (pidOutput[i] >= millis() - pidStart) {
      digitalWrite(heaterPin[i], HIGH);
    }
    else {
      digitalWrite(heaterPin[i], LOW);      
    }
  }
  
}

//******************************************************************************************************************************
//  INTLENGTH: GET THE LENGTH OF A INTEGER
//******************************************************************************************************************************
int intLength(int myInt) {

  myInt = abs(myInt);

  if (myInt >= 10000) {
    return 5;
  }
  else if (myInt >= 1000) {
    return 4;
  }
  else if (myInt >= 100) {
    return 3;
  }
  else if (myInt >= 10) {
    return 2;
  }
  else {
    return 1;
  }
  
}

//******************************************************************************************************************************
//  READTEMPS: Read the temperatures
//******************************************************************************************************************************
void readTemps() {

  // Loop thru all zones
  for (i = 0; i < numZones; i++) {
    if (tempScale == 'C') {
      pidInput[i] = thermo[i].readCelsius();
    }
    if (tempScale == 'F') {
      pidInput[i] = thermo[i].readFahrenheit();
    }    
  }

}

//******************************************************************************************************************************
//  SETUPPIDS: INITIALIZE THE PID LOOPS
//******************************************************************************************************************************
void setupPIDs() {

  for (i = 0; i < numZones; i++) {
    pidCont[i].SetSampleTime(pidCycle);
    pidCont[i].SetOutputLimits(0, pidCycle);
    pidCont[i].SetMode(AUTOMATIC);
  }

}

//******************************************************************************************************************************
//  SHUTDOWN: SHUT DOWN SYSTEM
//******************************************************************************************************************************
void shutDown() {

  // Turn off all zones (heating element relays)
  for (i = 0; i < numZones; i++) {
    digitalWrite(heaterPin[i], LOW);
  }
  
  // Disable interrupts / Infinite loop
  cli();
  while (1);
  
}

//******************************************************************************************************************************
//  UPDATEPIDS: UPDATE THE PID LOOPS
//******************************************************************************************************************************
void updatePIDs() {

  // Get the last target temperature
  if (segNum == 1) {  // Set to room temperature for first segment
    if (tempScale == 'C') {
      lastTemp = 24;
    }
    if (tempScale == 'F') {
      lastTemp = 75;
    }
  }
  else {
    lastTemp = segTemp[segNum - 2];
  }

  // Calculate the new setpoint value.  Don't set above / below target temp  
  if (segPhase == 0) {
    rampHours = (millis() - rampStart) / 3600000.0;
    calcSetPoint = lastTemp + (segRamp[segNum - 1] * rampHours);  // Ramp
    if (segRamp[segNum - 1] >= 0 && calcSetPoint >= segTemp[segNum - 1]) {
      calcSetPoint = segTemp[segNum - 1];
    }
    if (segRamp[segNum - 1] < 0 && calcSetPoint <= segTemp[segNum - 1]) {
      calcSetPoint = segTemp[segNum - 1];
    }
  }
  else {
    calcSetPoint = segTemp[segNum - 1];  // Hold
  }

  // Read the temperatures
  readTemps();
  
  // Loop thru all PID controllers
  for (i = 0; i < numZones; i++) {

    // Set the target temp.  Add any offset.
    pidSetPoint[i] = calcSetPoint + tempOffset[i];

    // Update the PID based on new variables
    pidCont[i].Compute();

  }

}

//******************************************************************************************************************************
//  UPDATESEG: UPDATE THE PHASE AND SEGMENT
//******************************************************************************************************************************
void updateSeg() {

  // Start the hold phase
  if ((segPhase == 0 && segRamp[segNum - 1] < 0 && pidInput[0] <= (segTemp[segNum - 1] + tempRange)) || 
      (segPhase == 0 && segRamp[segNum - 1] >= 0 && pidInput[0] >= (segTemp[segNum - 1] - tempRange))) {
    segPhase = 1;
    holdStart = millis();
  }

  // Go to the next segment
  if (segPhase == 1 && millis() - holdStart >= segHold[segNum - 1] * 60000) {
    segNum = segNum + 1;
    segPhase = 0;
    rampStart = millis();
  }

  // Check if complete / turn off all zones
  if (segNum - 1 > lastSeg) {
    for (i = 0; i < numZones; i++) {
      digitalWrite(heaterPin[i], LOW);
    }
    screenNum = 4;
  }

}



