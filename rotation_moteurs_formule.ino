//setup
#include <Servo.h>
Servo motor;

//réglages du suivi
const String heure = "07/03/2018 14:05:00";
double decl; //radians
double ha;
double az;

//position du moteur
int pos = 1;
const int bob1 = 4;
const int bob2 = 5;
const int bob3 = 6;
const int bob4 = 7;
const int bob5 = 8;
const int bob6 = 9;
const int bob7 = 10;

//sensibilité des moteurs 
const int delayServo = 20;
const int delayStep = 10;

//initialisation du système de délai
int pasServo = 0;
int pasStep = 0;

//fonction definissant les positions du moteur pas à pas
void writeStep(int s){
  switch(s){
  case 1:
    digitalWrite(bob1,LOW);
    digitalWrite(bob2,LOW);
    digitalWrite(bob3,HIGH);
    digitalWrite(bob4,HIGH);
    digitalWrite(bob5,HIGH);
    digitalWrite(bob6,LOW);
    digitalWrite(bob7,LOW);
    break;
  case 2:
    digitalWrite(bob1,LOW);
    digitalWrite(bob2,LOW);
    digitalWrite(bob3,LOW);
    digitalWrite(bob4,LOW);
    digitalWrite(bob5,HIGH);
    digitalWrite(bob6,HIGH);
    digitalWrite(bob7,LOW);
    break;
  case 3:
    digitalWrite(bob1,LOW);
    digitalWrite(bob2,LOW);
    digitalWrite(bob3,LOW);
    digitalWrite(bob4,LOW);
    digitalWrite(bob5,LOW);
    digitalWrite(bob6,HIGH);
    digitalWrite(bob7,HIGH);
    break;
  case 4:
    digitalWrite(bob1,HIGH);
    digitalWrite(bob2,HIGH);
    digitalWrite(bob3,HIGH);
    digitalWrite(bob4,HIGH);
    digitalWrite(bob5,LOW);
    digitalWrite(bob6,LOW);
    digitalWrite(bob7,HIGH);
    break;
  }
}

//initialisation du programme
void setup() {
  pinMode(bob1,OUTPUT);
  pinMode(bob2,OUTPUT);
  pinMode(bob3,OUTPUT);
  pinMode(bob4,OUTPUT);
  pinMode(bob5,OUTPUT);
  pinMode(bob6,OUTPUT);
  pinMode(bob7,OUTPUT);
  motor.attach(3);

  writeStep(1);
  solarPos(heure, 1, 42.41, 2.89, &ha, &az);
  motor.write(75);
  Serial.begin(9600);
}

//code executé en boucle
void loop() {
  pasStep = (pasStep+1)%delayStep;
  pasServo = (pasServo+1)%delayServo;

  if(pasStep == 0) testStep();
  if(pasServo == 0) testServo();
  
  delay(1);
}

//teste s'il faut faire tourner le moteur pas à pas
void testStep(){
  if(analogRead(0) < 500){
    pos = (pos==4)? 0 : pos+1;
    writeStep(pos);
  } 
  else if(analogRead(0) > 600){
    pos = (pos==0)? 4 : pos-1;
    writeStep(pos);
  }
}

//teste s'il faut faire tourner le servo moteur
void testServo(){
  if(analogRead(1) < 500 || analogRead(1) > 600){
    motor.write(155+decl*180/PI*-1);
    Serial.println(String(155+decl*180/PI*-1));
  } 
}

// Les calculs suivants sont bas�s sur le document : https://www.esrl.noaa.gov/gmd/grad/solcalc/solareqns.PDF

#define PI_180  0.017453292
#define A180_PI 57.29577951

int nbJmois[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
double eqT; // minutes

String conv_Mn_HMS(double mn) { // retourne l'heure, les minutes et les secondes sous forme d'une cha�ne au format : HH:MM:SS
  int hr = mn / 60;
  int min = trunc(mn - 60 * hr);
  int sec = trunc((mn - trunc(mn)) * 60);
  char stx[10]; // stockage de la cha�ne compl�te
  sprintf(stx, "%02u:%02u:%02u", hr, min, sec);
  return stx;
}

int jourAnnee(int a, int m, int j) { // retourne le jour de l'ann�e "a" au mois "m" et au jour "j"
  int nbJ = 0;
  for (byte i = 0; i < m; i++)
    nbJ += nbJmois[i];
  if (a % 4 == 0 && m > 2) // Ca s d'une ann�e bissextile
    nbJ++;
  return nbJ + j;
}

double fracAnnee(int a, int m, int j, int h, int mn, int s) { // fraction de l'ann�e en radians le jour de l'ann�e "a", au mois "m", au jour "j", � l'heure "h", la minute "mn" et la seconde "s"
  int n = 365;
  if (a % 4 == 0) // Cas de l'ann�e bissextile
    n = 366;
  return 2 * PI * (jourAnnee(a, m, j) - 1 + (h + mn / 60 + s / 3600 - 12) / 24) / n; // radians
}

void eqT_Decl(double fA) { // retourne l'�quation du temps en minutes et l'angle de d�clinaison du soleil en radians
  eqT = 229.18 * (0.000075 + 0.001868 * cos(fA) - 0.032077 * sin(fA) - 0.014615 * cos(2 * fA) - 0.040849 * sin(2 * fA)); // minutes
  decl = 0.006918 - 0.399912 * cos(fA) + 0.070257 * sin(fA) - 0.006758 * cos(2 * fA) + 0.000907 * sin(2 * fA) - 0.002697 * cos(3 * fA) + 0.00148 * sin(3 * fA); // radians
}

void solarPos(int a, int m, int j, int h, int mn, int s, int tZ, double la, double lon, double *ha, double *az) {
  // "tZ" est la zone horaire du lieu
  // En plus de donn�es calendaires vues ci-avant, "la" et "lon" sont respectivement la latitude et la longitude du lieu en degr�s d�cimaux.
  // La latitude est comprise entre - 90 � et +90 �. Un angle positif caract�rise l'h�misph�re Nord
  // "ha" est la hauteur  du soleil au-dessu de l'horizon. "az" est l'azimut du soleil compt� positivement dans le sens des aiguilles d'une motre � partir du Nord
  //"ha" et "az" doivent �tre transmis par adresse
  eqT_Decl(fracAnnee(a, m, j, h, mn, s));
  double tOff = eqT + 4.0 * lon - 60.0 * tZ; // minutes
  double tSv = h * 60.0 + mn + s / 60.0 + tOff; // minutes
  double aS = tSv / 4.0 - 180; // degr�s
  la = la * PI_180; // radians
  lon = lon * PI_180; // radians
  double ze = acos(sin(la) * sin(decl) + cos(la) * cos(decl) * cos(aS * PI_180)); // radians
  double cs = -(sin(la) * cos(ze) - sin(decl)) / (cos(la) * sin(ze)); // radians
  if (cs < -1) // protection contre les impr�cisions de calcul
    cs = -1;
  else if (cs > 1)
    cs = 1;
  *az = acos(cs) * A180_PI; // degr�s
  if (aS > 0)
    *az = 360 - *az;
  *ha = 90 - ze * A180_PI; // degr�s
}

void solarPos(String dh, int tZ, double la, double lon, double *ha, double *az) {
  // M�me fonction que le pr�c�dente sauf que la date et l'heure sont fournies au format ci-apr�s
  // dh est au format : JJsMMsAAAAsHHsMMsSS o� s est un s�parateur quelconque de un caract�re
  int a = dh.substring(6, 10).toInt();
  int m = dh.substring(3, 5).toInt();
  int j = dh.substring(0, 2).toInt();
  int h = dh.substring(11, 13).toInt();
  int mn = dh.substring(14, 16).toInt();
  int s = dh.substring(17).toInt();
  solarPos(a, m, j, h, mn, s, tZ, la, lon, ha, az);

}

