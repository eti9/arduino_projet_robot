void intro() {
  //Cette fonction sert tous simplement a afficher le texte d'introduction sur l'écran LCD
  lcd.setCursor(0, 0);
  lcd.print("Prog. Robot V1.0");
  lcd.setCursor(0, 1);
  lcd.print("Etien Robert A16");
}
void afficheDonnee(String sligne, int vitRG, int vitRD) {
  //Cette fonction sert a afficher les données du capteurs *sous forme d'une ligne de 5 digit* et la vitesse en pourcentage des moteurs
  int y = 14;
  if (i == false) {
    lcd.setCursor(5, 0);
    lcd.print(sligne);
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("   PRESS HIGH   ");
  }
  else {
    lcd.setCursor(5, 0);
    lcd.print(sligne);
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(vitRG);
    lcd.print("%");
    if (vitRD > 10) y--;
    if (vitRD > 100) y--;
    lcd.setCursor(y, 1);
    lcd.print(vitRD);
    lcd.print("%");
  }
}
void afficheSW(bool swG, bool swD) {
  //Cette fonction permet d'affiché sur l'écran LCD l'état des parchocs (ouvert ou fermé)
  lcd.setCursor(0, 0);
  lcd.print("SW_G        SW_D");
  lcd.setCursor(0, 1);
  switch (swG) {
    case 0:
      lcd.print("_/_");
      break;
    case 1:
      lcd.print("___");
      break;
  }
  lcd.setCursor(13, 1);
  switch (swD) {
    case 0:
      lcd.print("_/_");
      break;
    case 1:
      lcd.print("___");
      break;
  }
}
void afficheCHOIX() {
  //Cette fonction, tout dépendant de la valeur de choixDelAffichage, va executé une fonction d'affichage de l'écran LCD
  switch (choixDelAffichage) {
    case 0:
      //Ici, le texte d'introduction
      intro();
      break;
    case 1:
      //Ici, les valeurs du capteurs et des moteurs
      afficheDonnee(ligne, vitesseG, vitesseD);
      break;
    case 2:
      //Ici, l'état des interrupteur des parchocs
      afficheSW(pcG, pcD);
      break;
  }
}
void boutonClavier() {
  /* Cette fonction gere les boutons qui eux même la selection
      de l'affichage et la fonction marche/arrêt du robot
  */
  int etatSELECTION = clavier.etatBouton(B_SELECTION);
  if  (etatSELECTION == ENFONCANT) {
    lcd.clear();
    choixDelAffichage++;
    if (choixDelAffichage == 3) choixDelAffichage = 0;
  }

  int etatHAUT = clavier.etatBouton(B_HAUT);
  if  (etatHAUT == ENFONCANT) {
    i = !i;
  }
}
/*
   Ici c'est le gros du programme, on va analysé
   le capteur et définir la vitesse a laquel les
   moteurs doivent aller par rapport a celle-ci.
   J'utilise un algorithme assez simple : on prend la position du premier 1 dans un lecture de gauche a droite,
   puis on double cette valeur et on la soustrait au nombre de 0. Puis apres je fais plusieurs m
   anipulation pour rendre ce code utilisable et rejeter les mauvaise données.
   Donc, si j'ai 01100; ca me donne donc (1) comme position  pour le premier 1 et (3) 0. donc on obtien 3-2 = 1;
   Ce qui nous donne une possibilité de -4, -3, -2, -1, 0, 1, 2, 3, 4.
   -4 étant seulement le moteur de gauche qui fonctionne.
   4 étant seulement le moteur de droite qui fonctionne.
   0 étant les deux moteurs a 75% (théoriquement).
   et je fait des bon de 25% de la valeur du calibrage par case.
   Plus d'explication dans le rapport.

*/
void calculLigneMoteur() {
  int result;
  int nbDeTrue = 0;
  int compteurDeFalse = 0;
  int positionDuTrue = 0;
  bool ligneNumerique[5] = {0, 0, 0, 0, 0};
  bool activeornot = true;
  for (int c = 0; c < 5; c++) {
    switch (ligne.charAt(c)) {
      case '1':
        ligneNumerique[c] = true;
        break;
      case '0':
        ligneNumerique[c] = false;
        break;
    }
  }
  for (int c = 0; c < 5; c++) {
    if (ligneNumerique[c]) {
      nbDeTrue++;
      if (activeornot) {
        activeornot = !activeornot;
        positionDuTrue = c;
      }
    }
    else if (!ligneNumerique[c]) compteurDeFalse++;
  }
  result = compteurDeFalse - (2 * positionDuTrue);
  if (compteurDeFalse == 4 || (compteurDeFalse == 3 && ligneNumerique[(positionDuTrue + 1)])) {
    //Ici je convertie mes resultat negatif en positif et je retire le sens (si négatif alors moteur droit influencé si positif alors moteur gauche)
    //Je ramene cette valeur a 1 ou 0 afin de pouvoir utilisé mon tableau de pointeur. 
    //La fonction sqrt a la particuliarité de renvoyé 0 sur un int lorsqu'elle se fait sur une valeur négative. elle renvoie aussi un entier quand la valeur est plus grande que 0.
    //Il me suffit juste de convertir l'entier obtenu en bool pour avoir un 1 et un 0 soit les deux entrées du tableau
    int squareTest = sqrt(result);
    bool squareResult = squareTest;
    int negativeToPositive = result * result;
    result = sqrt(negativeToPositive);

    //Ici on utilise les tableaux de pointeur pour déterminer quel moteur et a quel vitesse il va
    *ptVitesse[squareResult] = *ptCalibrage[squareResult];
    *ptVitesse[!squareResult] = *ptCalibrage[!squareResult] - (*ptCalibrage[!squareResult] * (0.25 * result));
  }
  if (compteurDeFalse == 5 || nbDeTrue == 5) {
    vitesseD = 0;
    vitesseG = 0;
  }
}
