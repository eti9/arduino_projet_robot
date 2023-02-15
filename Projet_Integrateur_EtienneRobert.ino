/* Auteur : Étienne Robert

   Date : 12 Décembre 2016

   Description : Projet intégrateur permettant de faire rouler un robot selon un trajet noir (ruban docktape) sur le sol, comprenant un affichage LCD et deux parchocs.

   Compilé et testé avec la version Arduino 1.6.12
   Testé et calibré avec le robot N.01

*/
//Inclure les bibliothèque nécéssaire pour notre programme
#include "Robot.h"
#include "SPI.h"
#include "ClavierGenerique.h"
#include "Clavier.h"
#include "ClavierACL.h"
#include "EEPROM.h"
#include "LiquidCrystal.h"

//Nos variable constante de calibration (Si les moteurs vont a la même vitesse alors les deux devraient avoir une valeur de 75 pour 75% de la vitesse maximal du robot).
const int calibrageGAUCHE = 75;
const int calibrageDROITE = 75;

//Déclaration de nos variables qu'on vas utilisé lors de notre programme
int vitesseD = 0;
int vitesseG = 0;
int choixDelAffichage = 1;
bool pcG;
bool pcD;
boolean i = false;
String ligne = "11111";
String ligneprecedente = "11111";
int *ptVitesse[2] = { &vitesseG, &vitesseD};
const int *ptCalibrage[2] = { &calibrageGAUCHE, &calibrageDROITE};

//Déclaration de nos fonction
void  intro();
void afficheDonnee(String , int , int );
void afficheSW(bool swG, bool swD);
void afficheCHOIX();
void boutonClavier();
void calculLigneMoteur();

//Déclarer notre robot, notre affichage LCD et nos boutons
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
Robot robot;
ClavierACL clavier;

void setup() {
  //Initialisation du robot, on affiche notre texte d'introduction pendant une seconde
  lcd.begin(16, 2);
  intro();
  delay(2000);
  lcd.clear();
}

void loop() {
  //Programme principale

  //Ici on met a jour nos variables et notre robot
  robot.actualiser();
  ligne = robot.suiveur.lire();
  clavier.actualiser();
  pcG = robot.parechoc.lire(PC_GAUCHE);
  pcD = robot.parechoc.lire(PC_DROIT);

  //Voir la description de la fonction
  boutonClavier();

/*
 *   Les moteurs sont a 0% lorsque les parchocs
     sont enfoncés ou si (i), qui est dépendant du bouton HIGH,
     renvoie 0.
 */

  if (i && !pcG && !pcD) {
    calculLigneMoteur();
    robot.moteurs.tourner(vitesseG, vitesseD);
  }
  else {
    vitesseG = 0;
    vitesseD = 0;
    robot.moteurs.tourner(0, 0);
  }

  //Voir description de la fonction
  afficheCHOIX();
}
