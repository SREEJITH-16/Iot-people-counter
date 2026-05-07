#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

int entrySensor = 2;
int exitSensor = 3;
int buzzer = 4;

int count = 0;
int limit = 10;

int inCount = 0;
int outCount = 0;

bool entryState = false;
bool exitState = false;

void setup()
{
 Serial.begin(9600);

 pinMode(entrySensor, INPUT);
 pinMode(exitSensor, INPUT);
 pinMode(buzzer, OUTPUT);

 lcd.init();
 lcd.backlight();


 /* BOOT SCREEN 1 */

 lcd.setCursor(3,0);
 lcd.print("TEAM NOVA");

 delay(2000);

 lcd.clear();


 /* BOOT SCREEN 2 */

 lcd.setCursor(0,0);
 lcd.print("PEOPLE COUNTER");

 delay(2000);

 lcd.clear();

 updateLCD();
}

void loop()
{

 int entryValue = digitalRead(entrySensor);
 int exitValue = digitalRead(exitSensor);

 // ENTRY
 if(entryValue == LOW && entryState == false)
 {
   if(count < limit)
   {
     count++;
     inCount++;
     beep();
   }
   else
   {
     fullAlarm();
   }

   entryState = true;
   updateLCD();
   delay(500);
 }

 if(entryValue == HIGH)
 entryState = false;


 // EXIT
 if(exitValue == LOW && exitState == false)
 {
   if(count > 0)
   {
     count--;
     outCount++;
     beep();
   }

   exitState = true;
   updateLCD();
   delay(500);
 }

 if(exitValue == HIGH)
 exitState = false;

}


/* LCD DISPLAY */

void updateLCD()
{
 lcd.clear();

 lcd.setCursor(0,0);
 lcd.print("IN:");
 lcd.print(inCount);

 lcd.setCursor(9,0);
 lcd.print("OUT:");
 lcd.print(outCount);

 lcd.setCursor(0,1);
 lcd.print("Current:");
 lcd.print(count);

 if(count >= limit)
 {
   lcd.setCursor(12,1);
   lcd.print("FULL");
 }
}


/* BEEP */

void beep()
{
 digitalWrite(buzzer,HIGH);
 delay(100);
 digitalWrite(buzzer,LOW);
}


/* ROOM FULL ALARM */

void fullAlarm()
{
 for(int i=0;i<3;i++)
 {
   digitalWrite(buzzer,HIGH);
   delay(200);
   digitalWrite(buzzer,LOW);
   delay(200);
 }
}