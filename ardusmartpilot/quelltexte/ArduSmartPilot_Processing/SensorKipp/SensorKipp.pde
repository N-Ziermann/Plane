/*
 Dieser Quellcode baut auf Quellcode aus dem Buch "Rapid Android Development" von D. Sauter auf.
 Siehe http://danielsauter.com
 */

import ketai.sensors.*;

KetaiSensor sensor;
// In diesen Variablen werden später die x-,y- unc z-Beschleunigungswerte eingetragen.
// Initialisierung mit Wert 0, damit auch ohne SensorEvent Kreis gezeichnet wird.
float beschleunigungX=0, beschleunigungY=0, beschleunigungZ=0;

void setup()
{
  size(400, 400);
  sensor = new KetaiSensor(this);
  sensor.start();
  orientation(LANDSCAPE);
  textAlign(CENTER, CENTER); 
  textSize(24);
}

void draw()
{
  //Variablen für x- bzw. y-Position des Kreises
  int x=0, y=0;
  //Variablen für Kippwinkel um y- bzw. x-Achse
  int kippWinkelY=0, kippWinkelX=0;
  background(255, 255, 0);
  fill(255, 0, 0);
  //Trigonometrische Berechnung der Kippwinkel in Grad, mit (int) Umwandlung von float → int.
  kippWinkelX = (int)(atan(beschleunigungY/beschleunigungZ)*180/PI);
  kippWinkelY = (int)(atan(beschleunigungX/beschleunigungZ)*180/PI);
  //Umrechnen der Kippwinkel in xy-Koordinatenpositionen
  x = (int)map(kippWinkelX, -90, 90, 0, 400);
  y = (int)map(kippWinkelY, -90, 90, 0, 400);
  //Kreis an der Position (x,y) zeichnen
  ellipse(x, y, 50, 50);
  fill(0, 0, 0);
  //Kippwinkel als Text ausgeben
  text("Kippwinkel (°):\n" + //(3)
  "alpha x: " + kippWinkelX + "\n" +
    "alpha y: " + kippWinkelY + "\n", width/2, height/2);
}

//Diese Methode wird jedes Mal automatisch aufgerufen, wenn sich ein
//Sensorwert geändert hat.
void onAccelerometerEvent(float x, float y, float z)
{
  beschleunigungX = -x;
  beschleunigungY = -y;
  beschleunigungZ = -z;
}

