#include <KY040rotary.h>
#include <DynamixelSerial.h>
#include <Wire.h>                      
#include <LiquidCrystal_I2C.h>       

LiquidCrystal_I2C lcd(0x27,20,4);   
const int RotaryCLK = 2;  //CLK pin on the rotary encoder
const int RotaryDT = 4;   //DT pin on the rotary encoder
const int RotarySW = 3;   //SW pin on the rotary encoder (Button function)

//Defining variables for rotary encoder and button
int ButtonCounter = 0;    //counts the button clicks
int RotateCounter = 0;   //counts the rotation clicks
bool rotated = true;  //info of the rotation
bool ButtonPressed = false; //info of the button


//Statuses
int CLKNow;
int CLKPrevious;
int DTNow;
int DTPrevious;
int count =0;
// Timers
float TimeNow1;
float TimeNow2;

//statuses (1/true: ON, 0/false: OFF)
bool adultModeStatus = false;
bool toddlerModeStatus = false;
bool infantModeStatus = false;
  

void setup()

{
 Dynamixel.setSerial(&Serial); // &Serial - Arduino UNO/NANO/MICRO, &Serial1, &Serial2, &Serial3 - Arduino Mega
 Dynamixel.begin(1000000,2);  // Inicialize the servo at 1 Mbps and Pin Control 2
  delay(1000);
  lcd.init ();                      // initialize the lcd   
  lcd.backlight();
  
  //------------------------------------------------------
  
  lcd.setCursor(1,1);
  lcd.print(" UTILE VENTILATOR ");
  lcd.setCursor (4,3);  
  lcd.print("BY_SUKKUR_IBA");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Select the Modes:");
  lcd.setCursor(0,1);
  lcd.print("1.Adult");
  lcd.setCursor(0,2);
  lcd.print("2.Toddler");
  lcd.setCursor(0,3);
  lcd.print("3.Infant");
  delay(2000);
  lcd.clear();
  menu (); 
  delay(1000); // wait 1 sec
  
  //------------------------------------------------------
  //setting up pins  
  
   pinMode(2, INPUT);
   pinMode(3, INPUT_PULLUP); // FOR BUTTON
   pinMode(4, INPUT);

  //Store states
   CLKPrevious = digitalRead(RotaryCLK);
   DTPrevious = digitalRead(RotaryDT);
    
   attachInterrupt(digitalPinToInterrupt(RotaryCLK), rotate, CHANGE);
   attachInterrupt(digitalPinToInterrupt(RotarySW), buttonPressed, FALLING); //either falling or rising but never "change".

   TimeNow1 = millis(); //Start timer 1  
  
}

void loop()
{
 printLCD();
 ButtonChecker(); 
}
void ButtonChecker() //this is basically the menu part. keep track of the buttonpressed and rotatecounter for navigation
{
  if(ButtonPressed == true)
  {
    switch(RotateCounter)
    {
      case 1:      
      if(adultModeStatus == false)
      {
        adultModeStatus = true;
         adultModeOn(); // turn motors on in adult mode
      }
      else
      {
        adultModeStatus = false;
        adultModeOff(); // turn motors off
        lcd.clear();
        menu ();
      }

     lcd.setCursor(1,2); // Defining positon to write from second row, first column .
     lcd.print(adultModeStatus);
      
      break;
      
      case 2:
      if(toddlerModeStatus == false)
      {
        toddlerModeStatus = true;
        lcd.clear();
        lcd.setCursor(3,1);
        lcd.print("Toddler Mode");
        toddlerModeOn(); // turn motors in toddler mode
      }
      else
      {
        
       toddlerModeStatus = false;
       toddlerModeOff (); // turn motors off
       lcd.clear();
       menu();
       
      }
        lcd.setCursor(3,2); // Defining positon to write from second row, first column .
        lcd.print(toddlerModeStatus);  
      break;
      
      case 3:
      if(infantModeStatus == false)
      {
        infantModeStatus = true;
        lcd.clear();
        lcd.setCursor(3,1);
        lcd.print("Infant Mode");
        infantModeOn (); // turn Motors on in toddler mode
      }
      else
      {
       infantModeStatus = false;
       infantModeOff(); 
       lcd.clear();
       menu();
      }

      lcd.setCursor(5,2); // Defining positon to write from second row, first column .
      lcd.print(infantModeStatus);
      break;
     
    }    
  }  
  ButtonPressed = false; //reset this variable
}

void buttonPressed()
{  
  //This timer is a "software debounce". It is not the most effective solution, but it works
  
  TimeNow2 = millis();
  if(TimeNow2 - TimeNow1 > 500)
  {    
    ButtonPressed = true;
    ButtonCounter++;    
  }
  TimeNow1 = millis();  //"reset" timer; the next 500 ms is counted from this moment
}

void rotate()
{
  CLKNow = digitalRead(RotaryCLK); //Read the state of the CLK pin

  // If last and current state of CLK are different, then a pulse occurred  
    if (CLKNow != CLKPrevious  && CLKNow == 1)
    {
    // If the DT state is different than the CLK state then
    // the encoder is rotating CCW so increase
      if (digitalRead(RotaryDT) != CLKNow) 
      {        
      RotateCounter++; 

      if(RotateCounter > 3)
      {
       RotateCounter = 0;
      }

      }
      else
      {        
      RotateCounter--;
            
      if(RotateCounter < 0)
      {
        RotateCounter = 3;  
      }    
      }       
    }   
  CLKPrevious = CLKNow;  // Store last CLK state
  rotated = true;
}


void printLCD()
{
    if(rotated == true) //refresh the CLK
    { 
      lcd.setCursor(18,2); 
      lcd.print(RotateCounter);
      rotated = false;
    }
}

void menu() {
  
  lcd.setCursor(0,1); //Defining position to write from first row, first column .
  lcd.print(" SELECT MODE   MODE");
  lcd.setCursor(0,2); //second line, 1st block
  lcd.print(" 0 0 0            0"); //You can write 20 Characters per line .
}
void adultModeOn () {
    
      lcd.clear();
      lcd.setCursor(5,0);
      lcd.print("<Adult Mode>");
      lcd.setCursor(0,1);
      lcd.print("R.R= 12 to 16");
       lcd.setCursor(0,2);
      lcd.print("T.V= 400 - 700 mL/kg");
      lcd.setCursor(0,3);
      lcd.print("I/E= 1:2 - 1:3");


  do
  { 
  Dynamixel.setEndless(18,ON);
  Dynamixel.turn(18,LEFT,370);

  Dynamixel.setEndless(6,ON);
  Dynamixel.turn(6,LEFT,370);
  
  delay(1500);

  Dynamixel.setEndless(18,OFF);
  Dynamixel.setEndless(6,OFF);
  delay(1000);

  Dynamixel.setEndless(18,ON);
  Dynamixel.turn(18,RIGTH,80);

  Dynamixel.setEndless(6,ON);
  Dynamixel.turn(6,RIGTH,80);
 
  delay(1200);
  }
  while(digitalRead(3)==HIGH);

  adultModeOff ();

  
  
 
}


void infantModeOn () {


      lcd.clear();
      lcd.setCursor(5,0);
      lcd.print("<Infant Mode>");
      lcd.setCursor(0,1);
      lcd.print("R.R= 30 to 60"); // 44 is set by default
      lcd.setCursor(0,2);
      lcd.print("T.V= 400 - 600 mL/kg");
      lcd.setCursor(0,3);
      lcd.print("I/E= 1:2 - 1:4");

  do 
  { 
  Dynamixel.setEndless(18,ON);
  Dynamixel.turn(18,LEFT,700);

  Dynamixel.setEndless(6,ON);
  Dynamixel.turn(6,LEFT,700);
  
  delay(550);

  Dynamixel.setEndless(18,OFF);
  Dynamixel.setEndless(6,OFF);
  delay(200);

  Dynamixel.setEndless(18,ON);
  Dynamixel.turn(18,RIGTH,150);

  Dynamixel.setEndless(6,ON);
  Dynamixel.turn(6,RIGTH,150);

  delay(580);
}

  while(digitalRead(3)==HIGH);

  infantModeOff ();
}

void toddlerModeOn () {


      lcd.clear();
      lcd.setCursor(5,0);
      lcd.print("<Toddler Mode>");
      lcd.setCursor(0,1);
      lcd.print("R.R= 24 to 40");
      lcd.setCursor(0,2);
      lcd.print("T.V= 400 - 600 mL/kg");
      lcd.setCursor(0,3);
      lcd.print("I/E= 1:3 - 1:4");

  do 
  { 
  Dynamixel.setEndless(18,ON);
  Dynamixel.turn(18,LEFT,650);

  Dynamixel.setEndless(6,ON);
  Dynamixel.turn(6,LEFT,650);
  
  delay(550);

  Dynamixel.setEndless(18,OFF);
  Dynamixel.setEndless(6,OFF);
  delay(200);

  Dynamixel.setEndless(18,ON);
  Dynamixel.turn(18,RIGTH,100);

  Dynamixel.setEndless(6,ON);
  Dynamixel.turn(6,RIGTH,100);

  delay(550);
  
}

  while(digitalRead(3)==HIGH);

toddlerModeOff ();
}

void adultModeOff () {

  Dynamixel.setEndless(18,OFF);
  Dynamixel.setEndless(6,OFF);
 
 delay(3000);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Adult Mode OFF");
  menu();
 
}

void infantModeOff() {
  Dynamixel.setEndless(18,OFF);
  Dynamixel.setEndless(6,OFF);
 delay(3);
 
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Infant Mode OFF");
  menu();
}

void toddlerModeOff () {
  Dynamixel.setEndless(18,OFF);
  Dynamixel.setEndless(6,OFF);
 delay(3);

  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Toddler Mode OFF");
  menu();
}
