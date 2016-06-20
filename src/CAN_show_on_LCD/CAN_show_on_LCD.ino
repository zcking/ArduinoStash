// include the library code:
#include <LiquidCrystal.h>
#include "can_constants.h"

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

int id = 0;
int dlc = 0;
unsigned char data[8] = {0};

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  Serial.begin(115200);
  randomSeed(analogRead(0));
  // Print a message to the LCD.
  lcd.print("CAN init OK!");
  InitMessages();
}

void loop() {
  GenerateMessage(id, dlc, &data[0]);
  ShowMessage();
  delay(5000);
}

void ShowMessage()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ID: ");
  lcd.print(id, HEX);
  lcd.print(", DLC:");
  lcd.print(dlc);
  Serial.print("ID: ");
  Serial.println(id, HEX);
  lcd.setCursor(0, 1);
  for (int i = 0; i < dlc; i++)
  {
    if (data[i] < 0x10)
      lcd.print("0");
    lcd.print(data[i], HEX);
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println("\n-------------------------------------------------");
}

void GenerateMessage(int &id, int &dlc, unsigned char *data)
{
  int selector = random(3);
  id = SelectMessage(selector);
  dlc = random(9);
  for (int i = 0; i < dlc; i++)
  {
    data[i] = random(0x100);
  }
}
//void GenerateMessage(int &id, int &dlc, unsigned char *data)
//{
//    do
//    {
//      id = random(ID_RANGE) + MIN_ID; // id = random int between 0..0x7FF
//    } while (!ValidMessage(id));
//    dlc = random(9);  // dlc = random int between 0..8
//    for(int i = 0; i < dlc; i++)
//    {
//      data[i] = random(0x100); // each byte is random char between 0..0xFF
//    }
//}
