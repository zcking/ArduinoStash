/*
Adafruit Arduino - Lesson 12. Light and Temperature
*/

#include <LiquidCrystal.h>
#include "DHT.h"

#define DHTPIN 13
#define DHTTYPE DHT11

int lightPin = 1;

//                BS  E  D4 D5  D6 D7
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
DHT dht(DHTPIN, DHTTYPE);

void setup() 
{
  lcd.begin(16, 2);
  Serial.begin(9600);
  dht.begin();
}

void loop()
{
  // Read temperature as Celsius (the default)
  float tempc = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float tempf = dht.readTemperature(true);
  //         ----------------
  lcd.print("Temp         F  ");
  lcd.setCursor(6, 0);
  lcd.print(tempf);
  /*
  // Display Light on second row
  int lightReading = analogRead(lightPin);
  lcd.setCursor(0, 1);
  //         ----------------
  lcd.print("Light           ");  
  lcd.setCursor(6, 1);
  lcd.print(lightReading);
  */
  delay(500);
}
