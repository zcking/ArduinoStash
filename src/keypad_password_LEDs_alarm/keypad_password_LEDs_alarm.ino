#include <OnewireKeypad.h>
#include <Password.h>
#include "pitches.h"

// notes for alarm
int alarm[] = {
  NOTE_F5,
  NOTE_E5
};

// duration of each alarm beep, in ms
const int duration = 500;

char KEYS[]= {  // Define keys values of Keypad
  '1','2','3','4',
  '5','6','7','8',
  '9','A','B','C',
  'D','E','#','*'
};

const int successLedPin = 7;
const int failLedPin = 8;
const int alarmPin = 4;

int numTries = 0;
const int NOTES_IN_ALARM = 2;

OnewireKeypad <Print, 16> KP(Serial, KEYS, 4, 4, A0, 5000, 1000 );
Password password = Password( "1234" );

void setup ()
{
  Serial.begin(9600);

  KP.SetHoldTime(1000);
  KP.ShowRange();

  pinMode(successLedPin, OUTPUT);
  pinMode(failLedPin, OUTPUT);
  pinMode(alarmPin, OUTPUT);
}

void turnLedsOff()
{
  digitalWrite(successLedPin, LOW);
  digitalWrite(failLedPin, LOW);
}

void loop()
{
  char Key;
  byte KState = KP.Key_State();

  if (KState == PRESSED)
  {
    if ( Key = KP.Getkey() )
    {
      Serial << "Pressed: " << Key << "\n";
      switch (Key)
      {
        case '*': 
          checkPassword(); 
          password.reset();
          break;
        case '#': 
          password.reset(); 
          turnLedsOff();
          break;
        default: 
          password.append(Key);
      }
    }
  }
  else if (KState == HELD)
  {
    Serial << "Key:" << KP.Getkey() << " being held\n";
  }
}

void checkPassword()
{
  turnLedsOff();
  delay(50);
  
  if (password.evaluate())
  {
    Serial.println("Success");
    //Add code to run if it works
    digitalWrite(successLedPin, HIGH);
    numTries = 0;
    
  }
  else
  {
    Serial.println("Wrong");
    //add code to run if it did not work
    digitalWrite(failLedPin, HIGH);
    numTries++;
    if (numTries > 2) {
      // sound the alarm!
      int note = 0;
      while(true)
      {
        tone(alarmPin, alarm[note], duration);
        note = (note + 1) % NOTES_IN_ALARM;
        delay(duration);
        if (KP.Key_State() == PRESSED && KP.Getkey() == '#')
        {
          turnLedsOff();
          numTries = 0;
          digitalWrite(alarmPin, LOW); // turn alarm off
          break;
        }
      }
    }
  }

  delay(500);
}
