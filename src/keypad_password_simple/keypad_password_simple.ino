#include <OnewireKeypad.h>
#include <Password.h>

char KEYS[]= {  // Define keys values of Keypad
  '1','2','3','4',
  '5','6','7','8',
  '9','A','B','C',
  'D','E','#','*'
};

OnewireKeypad <Print, 16> KP(Serial, KEYS, 4, 4, A0, 5000, 1000 );
Password password = Password( "8765" );

void setup ()
{
  Serial.begin(9600);

  KP.SetHoldTime(1000);
  KP.ShowRange();
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
        case '*': checkPassword(); break;
        case '#': password.reset(); break;
        default: password.append(Key);
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
  if (password.evaluate())
  {
    Serial.println("Success");
    //Add code to run if it works
  }
  else
  {
    Serial.println("Wrong");
    //add code to run if it did not work
  }
}
