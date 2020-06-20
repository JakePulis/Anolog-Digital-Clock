
/* Code from Gijs Withagen:
 *
 * Use 3 PWM pins to drive motor

 * Choose 9,10,11 and 6,5,3
 * Had to change the prescaler wit setPwmFrequency, @TODO need to clean
 * this up. And have a challenge to get a repaired micros() and millis().
 * because I would like to use PJON for a 1 wire communication bus.
 *
 *  USE Arduino Library Version 1.8.7 which has modified wiring.c in the core!

 Comments Olivier:
 Version 00 Test:
 - Serial is not working!!! remove all Serial communication!
 - Nullen nur am anfang im Setup durchgehen. Funktioniert!
 - OLED ist auskommentiert, da es den Prozess massiv verlangsamt
 - Maximalgeschwindingkeit ist rund 8s pro Umdrehung (speed ca. 500 ist maximum welche verarbeitet werden kann)
 - solved on one print: A2 und A3 are floating, need a pull down resistor!
 Version 01 Slave:
 - Initialize all Pins and connect to I2C as slave
 - OLED library can be removed when the slave is working good enough 
 Version 02 Slave:
 - Put the pins for the motor as const. But does not improve the situation


Bugs:
 - Change print with a resistor to common of I2C selection Switch
 - Nullen: Exit when no hall signal is found after 2 turns. Error message!
 - When one handle is accelerating then this influences the speed of the other (MCU is overloaded)

 
 */

// ---------- INCLUDE LIBRARIES --------------------
#include <Arduino.h>
#include <MultiStepper.h>
#include <AccelStepper.h>
#include <MotorVID29.h> // modified library from Gijs Withagen
#include <Wire.h> //I2C Library

// ------------- OLED initialization (can be removed later) ------------------
#include <SPI.h> // OLED
#include <Adafruit_GFX.h> //OLED
#include <Adafruit_SSD1306.h> // OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ------------- CREATE STEPPER OBJECT ---------------
const int Mot1Pin1 = 9;
const int Mot1Pin2 = 10;
const int Mot1Pin3 = 11;
const int Mot2Pin1 = 3;
const int Mot2Pin2 = 5;
const int Mot2Pin3 = 6;

MotorVID29 motor1(360 * 3 * 4, true, Mot1Pin1, Mot1Pin2, Mot1Pin3); 
MotorVID29 motor2(360 * 3 * 4, true, Mot2Pin1, Mot2Pin2, Mot2Pin3);
AccelStepper stepper1(stepper1_fw, stepper1_bw); // Call own function for step forward and backward. Accellstepper gives just a return on one of those functions if a stepp is required
AccelStepper stepper2(stepper2_fw, stepper2_bw); // Call own function for step forward and backward. Accellstepper gives just a return on one of those functions if a stepp is required

// --------------- DEFINE PINS -------------------------
byte PosInPin = 0;  //"Pos reached in"
byte PosOutPin = 1; // "Pos reached out"
byte HallPin1=A0;   // Hall sensor 1
byte HallPin2=A1;   // Hall sensor 2


// ---------------- DEFINE VARIABLES --------------------
int Pos1 = 0; // Faktor um Position umzuschalten
int Pos2 = 0; // Faktor um Position umzuschalten
bool StpZero_1 = false;
bool StpZero_2 = false;
int sensorValue = 0; // initialize sensor value
int HallVal = 500; // measured value from Hall sensor
int LowestHallVal = 500; // saved, last lowest Hall sensor value to zero pointers
int I2C_Address = 0;


// ----------------------------------------------------------------------------
// ------------------------------  SETUP --------------------------------------
// ----------------------------------------------------------------------------
void setup() {

  // ------------ PINMODES -----------------------
	pinMode(HallPin1, INPUT); // Hall sensor 1
  pinMode(HallPin2, INPUT); // Hall sensor 2
  pinMode(PosOutPin,OUTPUT);  // "Pos reached out"
  pinMode(PosInPin,INPUT); // "Pos reached in"
  pinMode(A2,INPUT_PULLUP); //I2C Address
  pinMode(A3,INPUT_PULLUP);//I2C Address
  pinMode(2,INPUT_PULLUP);//I2C Address
  pinMode(4,INPUT_PULLUP);//I2C Address
  pinMode(7,INPUT_PULLUP);//I2C Address
  pinMode(8,INPUT_PULLUP);//I2C Address

  // -------- DEFINE I2C ADDRESS --------------
  if (digitalRead(A2)==LOW){I2C_Address+=1;}
  if (digitalRead(A3)==LOW){I2C_Address+=2;}
  if (digitalRead(2)==LOW){I2C_Address+=4;}
  if (digitalRead(4)==LOW){I2C_Address+=8;}
  if (digitalRead(7)==LOW){I2C_Address+=16;}
  if (digitalRead(8)==LOW){I2C_Address+=32;}
  
  // ---------- SET PRESCALER -----------------
  // Change timing of MCU (done by Gijs Withager). Arduino IDE version 1.8.7 with modified wire.c required for this
  setPrescaler(0, 1);
  setPrescaler(1, 1);
  setPrescaler(2, 1);


  // ---- initialize OLED ----
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    for(;;); // Don't proceed, loop forever
  }
  //Debugg
  PrintDisplayText3Lines("I2C : ",I2C_Address,"HALL 1",analogRead(A0),"HALL 2",analogRead(A1));
  //  delay(1500);

  // ----------------- INITIALIZE MOTORS ----------------------
	motor1.setPosition(0); // Set position to zero
	motor2.setPosition(0); // Set position to zero
//  if (StpZero_1 == false) {Nullen1();} // go to Hall sensor 1 and drive back to correct pos 0
//  if (StpZero_2 == false) {Nullen2();} // go to Hall sensor 2 and drive back to correct pos 0

  // -------------- DEFINITION OF START BOUNDARIES FOR MOTORS ------------
	stepper1.setMaxSpeed(1080); // 1.0 = max speed, 0.9999 = extrem langsam! Wie zwischenstufen??
	stepper2.setMaxSpeed(1080); // 1.0 = max speed, 0.9999 = extrem langsam! Wie zwischenstufen??
	stepper1.setAcceleration(600); // 1.0 gut, 0.5 langsamere Beschleunigung
	stepper2.setAcceleration(600); // 1.0 gut, 0.5 langsamere Beschleunigung
  PrintDisplayText(0,"  RUN FOREST, RUN");


//  stepper1.setSpeed(1700); // debugg, only with function runSpeed instead of run
//  stepper1.setMinPulseWidth(20);
}


// ----------------------------------------------------------------------------
// ------------------------------  LOOP ---------------------------------------
// ----------------------------------------------------------------------------
void loop(void)
{
//  PrintDisplayHallVal(); // Debug
  stepper1.run();
//  stepper1.runSpeedToPosition();
  
	stepper2.run();

	if (stepper1.distanceToGo() == 0) {
		stepper1.moveTo(9000 * Pos1);
		if (Pos1 == 1) { Pos1 = -1; }
		else { Pos1 = 1; }
	}

  if (stepper2.distanceToGo() == 0) {
    stepper2.moveTo(3000 * Pos2);
    if (Pos2 == 1) { Pos2 = -1; }
    else { Pos2 = 1; }
  }
}


// ------------ CALLIBRATE POINTER 1 (upper) -------------------------------------
void Nullen1() {
  stepper1.setMaxSpeed(300.0); // 500 is maximum speed an ATMEGA328 can handle
	stepper1.setAcceleration(1000); // 1000 is high. To be checked
  PrintDisplayText(0,"CALIBRATE 1"); // Debugg
  delay(500); // Debugg
	LowestHallVal = 0; // Reset Lowest Hall measurement
  stepper1.move(9000); // move more than two turns to be sure to reach the Hall sensor
	do {
		stepper1.run();
		HallVal = analogRead(HallPin1);
		if (HallVal > LowestHallVal) {LowestHallVal = HallVal;}
/*      display.clearDisplay();
      display.setCursor(0,0);
      display.println(F("A0 > LAST-10"));
      display.setCursor(0,10);
      display.println(F("A0"));
      display.setCursor(70,10);
      display.println(HallVal);
      display.setCursor(0,20);
      display.println(F("LOWEST:"));
      display.setCursor(70,20);
      display.println(LowestHallVal);
      display.display();*/
//      delay(50);
	} while (HallVal > (LowestHallVal - 5)); // repeat as long as the Hall value is increasing, treshold 5 to ignore noise
	StpZero_1 = true; // set to calibrated
  PrintDisplayText(0,"ZEIGER 1 NULL"); // Debugg
  stepper1.setCurrentPosition(1520); // set position of hall sensor relative to true zero
  stepper1.moveTo(0);
  stepper1.setMaxSpeed(500.0); // Increase speed to go back to zero
  do {stepper1.run();} while (stepper1.distanceToGo()!=00); // Stepper 1 goes back to zero position

}

// ------------ CALLIBRATE POINTER 2 (lower) -------------------------------------
void Nullen2() {
  stepper2.setMaxSpeed(300.0); // 500 is maximum speed an ATMEGA328 can handle
  stepper2.setAcceleration(1000); // 1000 is high. To be checked
  PrintDisplayText(0,"CALIBRATE 2"); // Debug
  delay(500); // Debug
  LowestHallVal = 0; // Reset Lowest Hall measurement
  stepper2.move(9000); // move more than two turns to be sure to reach the Hall sensor
  do {
    stepper2.run();
    HallVal = analogRead(HallPin2);
    if (HallVal > LowestHallVal) {LowestHallVal = HallVal;}
/*      display.clearDisplay();
      display.setCursor(0,0);
      display.println(F("A1 > LAST-10"));
      display.setCursor(0,10);
      display.println(F("A1"));
      display.setCursor(70,10);
      display.println(HallVal);
      display.setCursor(0,20);
      display.println(F("LOWEST:"));
      display.setCursor(70,20);
      display.println(LowestHallVal);
      display.display();*/
  } while (HallVal > (LowestHallVal - 10));
  StpZero_2 = true;// set to calibrated
  PrintDisplayText(0,"Pointer 2 NULL"); // Debug
  stepper2.setCurrentPosition(1490); // set position of hall sensor relative to true zero
  stepper2.moveTo(0);
  stepper2.setMaxSpeed(500.0); // Increase speed to go back to zero
  do {stepper2.run();} while (stepper2.distanceToGo()!=00); // Stepper 2 goes back to zero position

}
