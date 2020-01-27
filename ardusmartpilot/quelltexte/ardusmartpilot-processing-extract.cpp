// Braucht man wegen der onCreate Callbackfunktion.
import android.os.Bundle;

// Ketai Bibliothek für die BT-Kommunikation.
// Quasi der Ersatz für die Serial Bibliothek
import ketai.net.bluetooth.*;

// Bibliothek für das Abfragen der Sensoren des Androidgeräts
import ketai.sensors.*;

// Konstanten für die Positionen der Buttons für Motorsteuerung
final static int XPOSBUT = 200, XSIZBUT = 100, YFIRBUT = 10, YDELBUT = 95, YSIZBUT = 80;
boolean bReleased = true; // Damit nur gesendet wird, wenn der Finger den Bildschirm wieder verlässt nach einer Berührung.

// String für Rückmeldung des Arduino
String echoArduino = "";

// Datenstring mit den Servowerten, der als Byte Array konvertiert über die Schnittstelle gesendet wird.
String datenString = "";
boolean dataEnd = false; // Flag, ob String komplett angekommen, da String von Arduino manchmal häppchenweise vom BT-Modul übertragen wird
KetaiBluetooth bt;
KetaiSensor sensor;

// In diesen Variablen werden später die x-,y- unc z-Beschleunigungswerte eingetragen.
// Initialisierung mit Wert 0, damit auch ohne SensorEvent Kreis gezeichnet wird.
float beschleunigungX=0, beschleunigungY=0, beschleunigungZ=0;
int seitenRuder = 90, hoehenRuder = 90, motorLeistung = 0; // Servowinkel in Grad, Motorleistung in %
String feedbackArdu = "", sensorArdu = "";

//***************************************************************************
// Dieser Quelltext wird für die Organisation der BT-Kommunikation gebraucht
//***************************************************************************
void onCreate(Bundle savedInstanceState) {
  super.onCreate(savedInstanceState);
  bt = new KetaiBluetooth(this); // Konstruktor
}

//***************************************************************************
// Jetzt kommt der eigentliche Quelltext!
//***************************************************************************
void setup()
{
  // Wenn Orientierung im manifest.xml festgelegt, dann diesen Befehl auskommentieren
  // orientation(LANDSCAPE);
  bt.start(); // BT Service starten.
  //***************************************************************
  // Hier muss die individuelle Adresse deines BT Moduls eingegeben werden.
  bt.connectDevice("00:13:01:06:20:83"); // HC-05 BT-Modul "SIA-Flug-2"
  //bt.connectDevice("00:11:12:11:04:73"); // BT-Shield Arduino
  //***************************************************************
  delay(2000);
  size(800, 480);
  sensor = new KetaiSensor(this);
  sensor.start();
  frameRate(20);
  echoArduino = new String();
}
void draw() {
  background(255, 255, 0);
  //Variablen für x- bzw. y-Position des Kreises
  int x=0, y=0;
  //Variablen für Kippwinkel um y- bzw. x-Achse
  int kippWinkelY=0, kippWinkelX=0;
  // Fadenkreuz in die Mitte des xy-Bildschirms zeichnen
  strokeWeight(2);
  line(550, 20, 550, 460);
  line(320, 240, 780, 240);
  //Trigonometrische Berechnung der Kippwinkel in Grad, mit (int) Umwandlung von float → int.
  kippWinkelX = (int)(atan(beschleunigungY/beschleunigungZ)*180/PI); //Kippen nach Rechts -> pos. Winkel
  kippWinkelY = (int)(atan(beschleunigungX/beschleunigungZ)*180/PI); //Kippen zum Betrachter hin -> pos. Winkel
  //Umrechnen der Kippwinkel in xy-Koordinatenpositionen
  x = (int)map(kippWinkelX, -90, 90, 320, 780);
  y = (int)map(kippWinkelY, -90, 90, 20, 460);
  //Kreis an der Position (x,y) zeichnen
  fill(255, 0, 0);
  ellipse(x, y, 30, 30);
  fill(0, 0, 0);
  if (mousePressed)
  //if ((mousePressed) && (bReleased == true)) //Wenn auf Bildschirm getippt wird
  {
    // Wenn x-Position des Klicks in der im Bereich der Buttons
    if ((mouseX>XPOSBUT)&&(mouseX<XPOSBUT+XSIZBUT))
    {
    // Je nach y-Position des Klicks, Auswahl Motorleisung und damit Button
      if ((mouseY>YFIRBUT+(0*YDELBUT))&&(mouseY<YFIRBUT+(1*YSIZBUT))) motorLeistung = 100;
      else if ((mouseY>YFIRBUT+(1*YDELBUT))&&(mouseY<YFIRBUT+(1*YDELBUT)+YSIZBUT)) motorLeistung = 75;
      else if ((mouseY>YFIRBUT+(2*YDELBUT))&&(mouseY<YFIRBUT+(2*YDELBUT)+YSIZBUT)) motorLeistung = 50;
      else if ((mouseY>YFIRBUT+(3*YDELBUT))&&(mouseY<YFIRBUT+(3*YDELBUT)+YSIZBUT)) motorLeistung = 25;
      else if ((mouseY>YFIRBUT+(4*YDELBUT))&&(mouseY<YFIRBUT+(4*YDELBUT)+YSIZBUT)) motorLeistung = 0;
    }
  }
  // Alle Buttons in Weiß darstellen
  fill(255, 255, 255);
  rect(XPOSBUT, YFIRBUT+ (0*YDELBUT),
   XSIZBUT,
   YSIZBUT);
  rect(XPOSBUT, YFIRBUT+ (1*YDELBUT),
   XSIZBUT,
   YSIZBUT);
  rect(XPOSBUT, YFIRBUT+ (2*YDELBUT),
   XSIZBUT,
   YSIZBUT);
  rect(XPOSBUT, YFIRBUT+ (3*YDELBUT),
   XSIZBUT,
   YSIZBUT);
  rect(XPOSBUT, YFIRBUT+ (4*YDELBUT),
   XSIZBUT,
   YSIZBUT);
  // Zuletzt geklickter Button (= Wert Motorleisung) grün darstellen
  fill (0, 255, 0);
  switch(motorLeistung)
  {
    case 100:
      rect(XPOSBUT, YFIRBUT+ (0*YDELBUT), XSIZBUT, YSIZBUT);
      break;
    case 75:
      rect(XPOSBUT, YFIRBUT+ (1*YDELBUT), XSIZBUT, YSIZBUT);
      break;
    case 50:
      rect(XPOSBUT, YFIRBUT+ (2*YDELBUT), XSIZBUT, YSIZBUT);
      break;
    case 25:
      rect(XPOSBUT, YFIRBUT+ (3*YDELBUT), XSIZBUT, YSIZBUT);
      break;
    case 0:
      rect(XPOSBUT, YFIRBUT+ (4*YDELBUT), XSIZBUT, YSIZBUT);
      break;
  }

  // Buttons mit Wert Motorleistung beschriften.
  textSize(24);
  fill(0, 0, 0);
  text("100 %", XPOSBUT+20, YFIRBUT+(0*YDELBUT)+50);
  text("75 %", XPOSBUT+20, YFIRBUT+(1*YDELBUT)+50);
  text("50 %", XPOSBUT+20, YFIRBUT+(2*YDELBUT)+50);
  text("25 %", XPOSBUT+20, YFIRBUT+(3*YDELBUT)+50);
  text("0 %", XPOSBUT+20, YFIRBUT+(4*YDELBUT)+50);
  
  // Anzeige Rückmeldung Arduino Zahl empfangener Werte
  text("Feedback", 50, 100);
  text("Arduino:", 50, 100+30);
  text(feedbackArdu, 50, 100+60);
  
  // Anzeige Rückmeldung Arduino Sensorwert
  text("Sensor-", 50, 300);
  text("wert:", 50, 300+30);
  text(sensorArdu, 50, 300+60);
  
  // Kippwinkel auf Bereich 0...180 Grad übertragen
  // Umrechnen in nötige in Seiten-/Höhenruderwerte (=Servowinkel) im Arduino.
  seitenRuder = (int)map(kippWinkelX, -90, 90, 0, 180);
  hoehenRuder = (int)map(kippWinkelY, -90, 90, 0, 180);
  // Der Datenstring wird nur dazu benutzt um die Zeichenkette mit den Kippwinkel und der Motorleistung zusammen zu
  // bauen und dann anschließend in einen Byte Array zu konvertieren.
  datenString += seitenRuder + "," + hoehenRuder + "," + motorLeistung +"\n"; // String erstellen mit den Servowinkeln
  // Datenstring elementweise (d.h.jedes char) in ein byte umwandeln. Da für die ASCII Zeichen nur
  // das erste Byte genutzt wird, geht dabei keine Information verloren. Der resultierende Byte Array
  // hat die gleiche "Länge" wie der String.
  byte[] daten = new byte[datenString.length()];
  for (int i=0; i<datenString.length (); i++) {
    daten[i] = byte(datenString.charAt(i));
  }
  bt.broadcast(daten); //Daten über BT senden. Da diese Methode nur bytes versendet, musste man vorher den string in ein
  byte-Array umwandeln.
  delay(10);
  datenString="";
}

void onBluetoothDataEvent(String who, byte[] data) //Callback Methode: Es liegen Daten vom Arduino an der BT-Schnittstelle an.
{
  //Falls noch kein \n empfangen wurde, String vom Arduino also nicht nicht komplett empfangen.
  if (!dataEnd) {
    //Byte Array elementweise in Char umwandeln und dem String hinzufügen.
    for (int i=0; i<data.length; i++ ) {
      echoArduino +=String.valueOf((char)data[i]);
    }
  }
  //Wenn das letzte Zeichen ein \n war, ist String vom Arduino komplett empfangen worden.
  if ((char)data[data.length-1]==&apos;\n&apos;) dataEnd = true;
  //Falls String vom Arduino komplett empfangen worden.
  if (dataEnd) {
    feedbackArdu = split(echoArduino, &apos;,&apos;)[0]; // Substring vor dem ersten Komma heraustrennen
    sensorArdu = split(echoArduino, &apos;,&apos;)[1]; // Substring zwischen ersten und zweiten Komma heraustrennen
    echoArduino=""; // Stingpufer zurück setzen.
    dataEnd = false; // Empfang des nächsten Strings ermöglichen.
  }
}

//Diese Methode wird jedes Mal automatisch aufgerufen, wenn sich ein
//Sensorwert geändert hat.
void onAccelerometerEvent(float x, float y, float z)
{
  beschleunigungX = -x;
  beschleunigungY = -y;
  beschleunigungZ = -z;
}

