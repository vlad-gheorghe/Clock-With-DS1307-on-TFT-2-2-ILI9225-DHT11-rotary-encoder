// Dawn & Dusk controller. http://andydoz.blogspot.ro/2014_08_01_archive.html
// 16th August 2014.
// (C) A.G.Doswell 2014
// adapted sketch by niq_ro from http://nicuflorica.blogspot.ro & http://arduinotehniq.blogspot.com/
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib


#include <Wire.h>
#include "RTClib.h" // from https://github.com/adafruit/RTClib

#include "SPI.h"

#include <UTFT.h>
//#define TFT_RST 8
//#define TFT_RS  9
//#define TFT_CS  10  // SS
//#define TFT_SDI 11  // MOSI
//#define TFT_CLK 13  // SCK
//#define TFT_LED 0   // 0 if wired to +5V directly
#include <Encoder.h> // from http://www.pjrc.com/teensy/td_libs_Encoder.html


RTC_DS1307 RTC; // Tells the RTC library that we're using a DS1307 RTC
Encoder knob(2, 3); //encoder connected to pins 2 and 3 (and ground)

UTFT lcd(QD220A,11,13,10,8,9); 
//the variables provide the holding values for the set clock routine
int setyeartemp; 
int setmonthtemp;
int setdaytemp;
int sethourstemp;
int setminstemp;
int setsecs = 0;
int maxday; // maximum number of days in the given month
int TimeMins; // number of seconds since midnight
int TimerMode = 2; //mode 0=Off 1=On 2=Auto
int TimeOut = 10;
int TimeOutCounter;
byte hh, mm, ss;

// These variables are for the push button routine
int buttonstate = 0; //flag to see if the button has been pressed, used internal on the subroutine only
int pushlengthset = 1000; // value for a long push in mS
int pushlength = pushlengthset; // set default pushlength
int pushstart = 0;// sets default push value for the button going low
int pushstop = 0;// sets the default value for when the button goes back high

int knobval; // value for the rotation of the knob
boolean buttonflag = false; // default value for the button flag


#include <DHT.h>
#define DHTPIN A1     // what pin we're connected DHT11/22
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22

DHT dht(DHTPIN, DHTTYPE);
float t1;
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];


void setup () {
    Serial.begin(57600); //start debug serial interface
    Wire.begin(); //start I2C interface
    RTC.begin(); //start RTC interface
  dht.begin();  // DHT init 

    lcd.InitLCD(); 
    lcd.clrScr(); 
    pinMode(A0,INPUT);//push button on encoder connected to A0 (and GND)
    digitalWrite(A0,HIGH); //Pull A0 high

    lcd.setBackColor(0,0,0);
   
    
  }
 
 
   
 
//}
           

void loop () {

  int h = dht.readHumidity();
   float t = dht.readTemperature();
  
 
   
   
    DateTime now = RTC.now(); //get time from RTC
    
 lcd.setBackColor(0,0,0);
 lcd.setColor(0,255 , 255);
   lcd.setFont(SmallFont);
 lcd.print("Today:",2,7);
 lcd.setFont(BigFont);
 //lcd.setColor(0,0 , 255);
   if (now.day() <10)
  {
     lcd.print("0"+String(now.day(), DEC),50,5);
     }else 

    lcd.print(String(now.day(), DEC),50,5);
    lcd.print("-",82,5);
  
   if (now.month() <10)
  {
     lcd.print("0"+String(now.month(), DEC),100,5);
     }else 

  lcd.print(String(now.month()),100,5);

    lcd.print("-",132,5);
    lcd.print(String(now.year(), DEC),150,5);
   if (t < 0) 
 {t1=-t;}
 else t1=t;
  if ( t1 < 10)
   {
     lcd.setBackColor(0,0,0); lcd.setFont(BigFont);lcd.setColor(255,0 , 255);
     lcd.print(" ",120,130); 
   }else
   if (t>0) lcd.print("+",75,130); else
   if (t==0) lcd.print(" ",75,130);else 
   if (t<0) lcd.print("-",75,130);else
   lcd.setColor(255,0 , 0);
  lcd.print(String(t1),90,130);
   
  lcd.setFont(SmallFont);
 lcd.print("o",175,128);
 lcd.print("Temp:",20,130);
  lcd.print("Umid:",20,150);
  
  lcd.setFont(BigFont);
   lcd.print("C",185,130);
    


   lcd.print(String(h)+".00",90,150);
   lcd.print("%RH",170,150); 

 
lcd.setFont(SevenSegNumFont);lcd.setColor(255,0 , 255);


  if (now.hour() <10)
  {
     lcd.print("0"+String(now.hour(), DEC),0,50);
     }else 

   lcd.print(String(now.hour(), DEC),0,50);
  
    if (now.minute() <10) 
      {
        lcd.print("0"+String(now.minute(), DEC),75,50);
     }else
     
    lcd.print(String(now.minute(), DEC),75,50);
   
    lcd.fillCircle(68, 60, 3);
     lcd.fillCircle(68, 90, 3);
     lcd.fillCircle(143, 60, 3);
     lcd.fillCircle(143, 90, 3);
    if (now.second() <10) 
    
      {
   //  lcd.setFont(SevenSegNumFontPlusPlus);
      lcd.print("0"+String(now.second(),DEC),150,50);
    
    }else
    lcd.print(String(now.second()),150,50);
  
    
     lcd.setFont(BigFont);
  
    if (TimerMode ==2) {
   
     }
   
    
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    delay (10);
    
    if (pushlength <pushlengthset) {
     
     // ShortPush ();   
    }
    
       
       //This runs the setclock routine if the knob is pushed for a long time
       if (pushlength >pushlengthset) {
         lcd.clrScr();
         DateTime now = RTC.now();
         setyeartemp=now.year(),DEC;
         setmonthtemp=now.month(),DEC;
         setdaytemp=now.day(),DEC;
         sethourstemp=now.hour(),DEC;
         setminstemp=now.minute(),DEC;
         setclock();
         pushlength = pushlengthset;
       };
}

//sets the clock
void setclock (){
   setyear ();
   lcd.clrScr ();
   setmonth ();
   lcd.clrScr ();
   setday ();
   lcd.clrScr ();
   sethours ();
   lcd.clrScr ();
   setmins ();
   lcd.clrScr();
   
   RTC.adjust(DateTime(setyeartemp,setmonthtemp,setdaytemp,sethourstemp,setminstemp,setsecs));

   delay (1000);
   
}

// subroutine to return the length of the button push.
int getpushlength () {
  buttonstate = digitalRead(A0);  
       if(buttonstate == LOW && buttonflag==false) {     
              pushstart = millis();
              buttonflag = true;
          };
          
       if (buttonstate == HIGH && buttonflag==true) {
         pushstop = millis ();
         pushlength = pushstop - pushstart;
         buttonflag = false;
       };
       return pushlength;
}
// The following subroutines set the individual clock parameters
int setyear () {
//lcd.clrScr();
  //  lcd.setCursor (0,0);
    lcd.print("Set Year",0,0);
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setyeartemp;
    }

 //   lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) { //bit of software de-bounce
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    setyeartemp=setyeartemp + knobval;
    if (setyeartemp < 2016) { //Year can't be older than currently, it's not a time machine.
      setyeartemp = 2016;
       if (setyeartemp > 2025) { //Year can't be older than currently, it's not a time machine.
      setyeartemp = 2025;
    }}
  //  char syt=setyeartemp;
    lcd.print(String(setyeartemp),0,40);
   // lcd.drawText(60,10,"  ",COLOR_WHITE); 
    setyear();
}
  
int setmonth () {
//lcd.clrScr();
  // lcd.setCursor (0,0);
    lcd.print("Set Month",0,0);
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setmonthtemp;
    }

 //   lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    setmonthtemp=setmonthtemp + knobval;
    if (setmonthtemp < 1) {// month must be between 1 and 12
      setmonthtemp = 1;
    }
    if (setmonthtemp > 12) {
      setmonthtemp=12;
    }
  // char smt=setmonthtemp;
    lcd.print(String(setmonthtemp),0,30);
   // lcd.drawText(60,10,"  ",COLOR_WHITE); 
    setmonth();
}

int setday () {
  if (setmonthtemp == 4 || setmonthtemp == 5 || setmonthtemp == 9 || setmonthtemp == 11) { //30 days hath September, April June and November
    maxday = 30;
  }
  else {
  maxday = 31; //... all the others have 31
  }
  if (setmonthtemp ==2 && setyeartemp % 4 ==0) { //... Except February alone, and that has 28 days clrScr, and 29 in a leap year.
    maxday = 29;
  }
  if (setmonthtemp ==2 && setyeartemp % 4 !=0) {
    maxday = 28;
  }
//lcd.clrScr();  
  // lcd.setCursor (0,0);
    lcd.print("Set Day",0,0);
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setdaytemp;
    }

//    lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    setdaytemp=setdaytemp+ knobval;
    if (setdaytemp < 1) {
      setdaytemp = 1;
    }
    if (setdaytemp > maxday) {
      setdaytemp = maxday;
    }
   // char sdt=setdaytemp;
    lcd.print(String(setdaytemp),0,30);
   // lcd.drawText(60,10,"  ",COLOR_WHITE); 
    setday();
}

int sethours () {
//lcd.clrScr();
   // lcd.setCursor (0,0);
    lcd.print("Set Hours",0,0);
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return sethourstemp;
    }

  //  lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    sethourstemp=sethourstemp + knobval;
    if (sethourstemp < 1) {
      sethourstemp = 1;
    }
    if (sethourstemp > 23) {
      sethourstemp=23;
    }
   // char sht=sethourstemp;
    lcd.print(String(sethourstemp),0,30);
    //lcd.drawText(60,10,"  ",COLOR_WHITE); 
    sethours();
}

int setmins () {
//lcd.clrScr();
  // lcd.setCursor (0,0);
    lcd.print("Set Mins",0,0);
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setminstemp;
    }

  //  lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    setminstemp=setminstemp + knobval;
    if (setminstemp < 0) {
      setminstemp = 0;
    }
    if (setminstemp > 59) {
      setminstemp=59;
    }
   // char smt=setminstemp,
    lcd.print(String(setminstemp),0,30);
  //  lcd.drawText(60,10,"  ",COLOR_WHITE); 
    setmins();
}

/*
 int setmode () { //Sets the mode of the timer. Auto, On or Off
lcd.clrScr();
   // lcd.setCursor (0,0);
    lcd.print(0,0,"Set Mode");
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return TimerMode;
    }

 //   lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    TimerMode=TimerMode + knobval;
    if (TimerMode < 0) {
      TimerMode = 0;
    }
    if (TimerMode > 2) {
      TimerMode=2;
    }
    if (TimerMode == 0) {
    lcd.print(90,300,"timer mode 0");
  //  lcd.drawText(90,0,"  ",COLOR_WHITE); 
    }
    if (TimerMode == 1) {
    lcd.print(90,30,"timer mode 1");
   // lcd.drawText(90,0,"  ",COLOR_WHITE); 
    }
    if (TimerMode == 2) {
    lcd.print(90,30,"timer mode 2");
   // lcd.drawText(90,0,"  ",COLOR_WHITE); 
    }
    setmode ();
    
}

*/
