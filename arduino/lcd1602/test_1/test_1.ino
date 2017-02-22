#include <Arduino.h>

/* LiquidCrystal Library - Hello World Demonstrates the use a 16x2 LCD display. The LiquidCrystal library works with all LCD displays that are compatible with the Hitachi HD44780 driver. There are many of them out there, and you can usually tell them by the 16-pin interface. This sketch prints "Hello World!" to the LCD and shows the time. 
The circuit: 
* LCD RS pin to digital pin 12 
* LCD Enable pin to digital pin 11 
* LCD D4 pin to digital pin 5 
* LCD D5 pin to digital pin 4 
* LCD D6 pin to digital pin 3 
* LCD D7 pin to digital pin 2 
* LCD R/W pin to ground 
* 10K resistor: * ends to +5V and ground 
* wiper to LCD VO pin (pin 3) 
This example code is in the public domain. 
http://www.arduino.cc/en/Tutorial/LiquidCrystal */
 
// on inclut la bibliothèque
#include <LiquidCrystal.h>
 
// définit un objet lcd avec les entrées sorties de l'Arduino en paramètre
//RS_pin EN_pin BUS1 BUS2 BUS3 BUS4
LiquidCrystal lcd(10, 11, 6,7, 8, 9);
 
void setup() {
 // défini le nombre de caractère et de ligne de l'écran
 lcd.begin(16, 2);
}
 
void loop() {
 //On va d'abord effacer tout les caractères sur l'écran:
 lcd.clear();
 lcd.setCursor(0, 0); //ici, on se positionne en haut à gauche de l'écran
 lcd.print("Hello, funLAB!!!"); //Le texte qu'on affiche sur l'écran à l'endroit positionné
 delay(500); //On attend 500ms 
 
// Après 500ms d'attente, on affiche la seconde ligne
 lcd.setCursor(0, 1); //On se place sur la deuxième ligne
 lcd.print("salut les geeks!");
 delay(500);
// On attends 500ms, puis on repart en haut de la fonction loop()
 
 }
 
