// In dieser Version ist ein Fehler der Ursprungsversion beseitigt worden:
// Es kommt vor, dass das BT-Modul den String vom Arduino nicht in einem Zug sondern in zwei Teilen sendet. 
// Die Ursache hierfür ist unklar. Es liegt vermutlich nicht an der eingestellten Baudrate. Möglicherweise ist
// dies ein Fehler des verwendeten Moduls HC-05 oder es liegt an der verwendeteten Callbackfunktion aus der
// Ketai-Bibliothek. Im Vergleich zur Vorversion liest diese Version die BT-Daten so 
// lange aus bis ein \n empfangen wird. Wird die Callbackfunktion "onBluetoothDataEvent" also zweimal ausgeführt,
// weil der String vom Arduino in zwei Teilen empfangen wird, dann wird dieser wieder zu einem String zusammen-
// gesetzt. Zusätzlich wird nicht mehr bei jedem Aufruf der Callbackfunktion eine neue String-Instanz geschaffen.
// Bei dieser Version muss das Smartphone beim Start der App hochkant ausgerichtet sein: Es ist ein bekannter Bug 
// von Processing, dass die setup() und onCreate() Methoden zweimal ausgeführt wer­den, wenn das Androidgerät beim 
// Start anders ausgerichtet ist. Das führt dazu, dass die BT-Verbindung zwar aufge­baut wird, aber keine BT-
// Kommunikation möglich ist. Die Version "ManifestBtTextEchoArduino" ist bei sonst identischem Quellcode dieser Bug 
// beseitigt:
// Im im Manifest sind die entsprechenden Befehle eingefügt worden, damit diese App nur im Landscape
// Modus startet. Daher ist der Befehl "orientation(PORTRAIT)" auskommentiert worden.
// S. Mack
// 20.02.15


import android.os.Bundle; // Braucht man wegen der onCreate Callbackfunktion.

// Ketai Bibliothek für die BT-Kommunikation.
// Quasi der Ersatz für die Serial Bibliothek
import ketai.net.bluetooth.*;

boolean bReleased = true; // Damit nur gesendet wird, wenn der Finger den Bildschirm wieder verlässt nach einer Berührung.
float abstand; // Abstand des "Klicks" zum Kreismittelpunkt
String echoArduino; // Pufferstring, in dem die via BT empfangen Zeichen hinein kommen.
KetaiBluetooth bt;
boolean dataEnd = false; // Flag, ob String komplett angekommen, da String von Arduino manchmal häppchenweise vom BT-Modul übertragen wird

//***************************************************************************
// Dieser Quelltext wird für die Organisation der BT-Kommunikation gebraucht
//***************************************************************************
void onCreate(Bundle savedInstanceState) {
  super.onCreate(savedInstanceState); 
  bt = new KetaiBluetooth(this);  // Konstruktor
}

//***************************************************************************
// Jetzt kommt der eigentliche Quelltext!
//***************************************************************************
void setup()
{
  // Zeile unten kann auskommentiert werden, wenn im Manifest im <activity> Element
  // android:screenOrientation="portrait" eingefügt wird. Dann funktioniert
  // BT Kommunikation auch bei Start im Portrait-Ausrichtung des Androidgeräts.

  orientation(PORTRAIT);

  bt.start(); // BT Service starten.
  //***************************************************************
  // Hier muss die individuelle Adresse deines BT Moduls eingegeben werden.
  //bt.connectDevice("00:11:12:11:04:73");
  bt.connectDevice("00:12:08:30:01:02");
  //***************************************************************
  size(400, 400);
  background(255, 255, 0);
  textSize(24); 
  frameRate(10);
  echoArduino = new String();
}

void draw()
{
  if ((mousePressed) && (bReleased == true)) //Wenn auf Bildschirm getippt wird
  {  
    background(255, 255, 0);
    abstand = dist(mouseX, mouseY, 200, 200);
    if (abstand <50) { // Antippen innerhalb des Kreises.
      fill(255, 0, 0);
      ellipse(200, 200, 100, 100);
      byte[] data = {
        'L', 'E', 'D', ' ', 'A', 'N', '\n'
      }; //Codewort für Arduino
      bt.broadcast(data); //Daten über BT senden
      bReleased = false; // Damit nicht gleich wieder Daten gesendet werden, wenn Finger noch auf Display
    } else { // Antippen außerhalb des Kreises.
      fill(0, 255, 0);
      ellipse(200, 200, 100, 100);
      byte[] data = {
        'L', 'E', 'D', ' ', 'A', 'U', 'S', '\n'
      }; //Codewort für Arduino
      bt.broadcast(data); //Daten über BT senden
      bReleased = false; // Damit nicht gleich wieder Daten gesendet werden, wenn Finger noch auf Display
    }
  }
  if (mousePressed == false) //Wenn Finger vom Display weg ist
  {
    bReleased = true; // Beim nächsten Berühren werden dann wieder Daten gesendet.
  }
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
  if ((char)data[data.length-1]=='\n') dataEnd = true;

  //Falls String vom Arduino komplett empfangen worden.
  if (dataEnd) {
    fill(0, 0, 0);
    text(echoArduino, 50, 100); // Empfangene Daten in schwarzer Schrift darstellen.
    echoArduino=""; // Stingpufer zurück setzen.
    dataEnd = false; // Empfang des nächsten Strings ermöglichen.
  }
}

