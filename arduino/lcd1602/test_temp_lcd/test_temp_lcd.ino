#include <Arduino.h>

#include <OneWire.h> // Inclusion de la librairie OneWire
// on inclut la bibliothèque
#include <LiquidCrystal.h>

#define DS18B20 0x28     // Adresse 1-Wire du DS18B20
#define BROCHE_ONEWIRE 12 // Broche utilisée pour le bus 1-Wire
 
// définit un objet lcd avec les entrées sorties de l'Arduino en paramètre
//RS_pin EN_pin BUS1 BUS2 BUS3 BUS4
LiquidCrystal lcd(10, 11, 6,7, 8, 9);
 
OneWire ds(BROCHE_ONEWIRE); // Création de l'objet OneWire ds
 
// Fonction récupérant la température depuis le DS18B20
// Retourne true si tout va bien, ou false en cas d'erreur
boolean getTemperature(float *temp){
  byte data[9], addr[8];
  // data : Données lues depuis le scratchpad
  // addr : adresse du module 1-Wire détecté
 
  if (!ds.search(addr)) { // Recherche un module 1-Wire
    ds.reset_search();    // Réinitialise la recherche de module
    return false;         // Retourne une erreur
  }
   
  if (OneWire::crc8(addr, 7) != addr[7]) // Vérifie que l'adresse a été correctement reçue
    return false;                        // Si le message est corrompu on retourne une erreur
 
  if (addr[0] != DS18B20) // Vérifie qu'il s'agit bien d'un DS18B20
    return false;         // Si ce n'est pas le cas on retourne une erreur
 
  ds.reset();             // On reset le bus 1-Wire
  ds.select(addr);        // On sélectionne le DS18B20
   
  ds.write(0x44, 1);      // On lance une prise de mesure de température
  delay(800);             // Et on attend la fin de la mesure
   
  ds.reset();             // On reset le bus 1-Wire
  ds.select(addr);        // On sélectionne le DS18B20
  ds.write(0xBE);         // On envoie une demande de lecture du scratchpad
 
  for (byte i = 0; i < 9; i++) // On lit le scratchpad
    data[i] = ds.read();       // Et on stock les octets reçus
   
  // Calcul de la température en degré Celsius
  *temp = ((data[1] << 8) | data[0]) * 0.0625; 
   
  // Pas d'erreur
  return true;
}

void setup() {
 // défini le nombre de caractère et de ligne de l'écran
 lcd.begin(16, 2);
  lcd.clear();
   lcd.setCursor(0, 0); //ici, on se positionne en haut à gauche de l'écran
 lcd.print("Hello world!!!"); //Le texte qu'on affiche sur l'écran à l'endroit positionné
}
 
void loop() {
    float temp;
    //On va d'abord effacer tout les caractères sur l'écran:


 delay(1000); 
 lcd.setCursor(0, 1); //On se place sur la deuxième ligne
  // Lit la température ambiante à ~1Hz
  if(getTemperature(&temp)) 
  {
    lcd.print("TEMP: ");
    lcd.print(temp);
  }
  else
  {
    lcd.print("error!!         ");
  }
  
 
 }
 
