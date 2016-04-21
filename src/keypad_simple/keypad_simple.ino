#include <OnewireKeypad.h> // OneWireKeypad Library

char KEYS[]= {  // Define keys values of Keypad
  '1','2','3','4',
  '5','6','7','8',
  '9','A','B','C',
  'D','E','F','G'
};

/* Define Library :
      OnewireKeypad <Print, #of buttons>
      Keypad(Serial, Char values, #Rows, #Cols, Arduino Pin, Row_resistor, Columns_resistor) */
OnewireKeypad <Print, 16 > Keypad(Serial, KEYS, 4, 4, A0, 5000, 1000 );

void setup ()
{
  Serial.begin(9600);
}

void loop() 
{
  Keypad.SetHoldTime(100);  // Key held time in ms
  
  if ((Keypad.Key_State() != 0)) {  // not pressed = 0, pressed = 1, released = 2,  held = 3
  
    char keypress = Keypad.Getkey();  // put value of key pressed in variable 'keypress'
    Serial.println("Keypad Key: ");
    Serial.println(keypress);  // Display value on Serial Monitor
    while ((Keypad.Key_State())){}  // Stay here while Key is held down
  }
  
}
