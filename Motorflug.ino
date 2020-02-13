/* Programm für Arduino Pro Mini am Flugzeug, das mit dem Processing
 Android Programm "BtSteuerungFlugzeugFinal" kommuniziert, das zyklisch
 folgende drei Strings sendet (mit Komma getrennt und mit "\n" als Abschluss-
 zeichen):
 Seitenruder in Grad (0...180), Höheruder in Grad (0...180), Motorleistung in Prozent (0...100), 
 
 Am Arduino Pro Mini sind die Anschlüsse wie folgt belegt:
 9 PWM-Signal Seitenruder, 10 PWM-Signal Höhenruder, 11 PWM-Signal Motorregler,
 A0 AD-Wandler für Sensorsignal
 
 Der Arduino meldet ein Datenprotokoll aus zwei Komma getrennten Strings zurück:
 Empfangene Kommaanzahl, Sensorwert \n
 In dieser Version wird statt des Sensorwertes der millis() Wert modulo 10000 zurückgegeben. 
 Beim Androidgerät wird dann die Zeit zwischen zwei empfangener solcher Zähler
 
 Der Quellcode basiert auf dem Programm ArduSteuerEcho aus dem Arduinokurs. 
 Baudrate auf 57600 erhöht. Dadurch flüssigere Servobewegung, aber Probleme beim Processingprogramm,
 die rückgemeldeten Werte ordentlich darzustellen.
 
 Werte für Höhenruder umgekert umgerechnet, damit bei Kippen zum Betrachter Auslenung des Ruders 
 nach Oben.
 Sicherheitsabfrage eingeführt: Fall über 2 s kein Signal empfangen, d.h. Flugzeug außerhalb BT-Reichweite:
 -> Motor aus, Höhenruder auf 0 und Seitenruder maximal einschlagen.
 Version 22. Dezember 2013
 */

#include <Servo.h> 
#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 2); // RX, TX

Servo seitenRuder, hoehenRuder, motorRegler;   // Variablen für die Servos (PWM-Signale).
int posSeiten = 0;                // Initialwerte.
int posHoehen = 0;
int leistMotor = 0;

int werte[3]; //Array für die beiden Integerwerte
int arrayIndex = 0; //Arrayindex initialisieren.
int kommaZahl = 0; //Anzal der empfangenen Werte
int sensorWert = 0; //Digitalwert des Sensors

//Zeit in ms für 10 empfangene Steuersignale (zum Erkennen Abbruch BT-Verbindung). Da das Processingprogramm
//derzeit mit frameRate=20 sendet, ist dieser Wert normal ca. 500 ms.
unsigned long zeitSteuerAlt = 0;
unsigned long zeitSteuerNeu = 0;

int zaehlSensor = 0;
boolean signalFehlt = false; //BT-Verbindung abgebrochen?

void setup() {
  // Nur zum Debuggen
  pinMode(13, OUTPUT);
  // !! Muss gleiche Baudrate sein wie beim HC-05 eingestellt!!
  // 115200 funktioniert bei ProMini 3,3V nicht zuverlässig!
  Serial.begin(57600);
  mySerial.begin(57600);
  seitenRuder.attach(6);  // PWM-Signal für Servo an Ausgang D9.
  hoehenRuder.attach(7); // PWM-Signal für Servo an Ausgang D10.
  motorRegler.attach(5); // PWM-Signal für Motorregler an Ausgang D11.
  seitenRuder.write(90);  // Servo zu Beginn auf 90 Grad stellen.
  hoehenRuder.write(90);
  //motorRegler.write(0); // Motor ausschalten
  
  motorRegler.write(180);
  delay(500);
  motorRegler.write(90);
  Serial.println("OK");  // Rückmeldung, dass Setup ausgeführt ist.
  // Bei gestarteter App muss dafür am Arduino die Reset Taste gedrückt werden.
}

void loop() {

  if( mySerial.available())
  {
    //Serial.println("hab was");
    char ch = mySerial.read();
    // Falls die ASCII Zeichen Ziffern sind.
    if(ch >= '0' && ch <= '9')
    {
      //Serial.println("hab eine Zahl");
      //ASCII Wert in Ziffer konvertieren und aus Ziffern Dezimalzahl machen.
      werte[arrayIndex] = (werte[arrayIndex] * 10) + (ch - '0'); 
    }
    else if (ch == ',') // Wenn Komma, dann zum nächsten Arrayelement wechseln.
    {
      arrayIndex = min(arrayIndex++,2); // Den Index auf maximal 2 begrenzen
      kommaZahl++;
      //Serial.println("hab ein Komma");
    }
    else if (ch =='e                                ') // Wenn letztes Zeichen Protokoll empfangen und zwei Werte da.
    {
      // Genaues Mapping hängt von (mechan.) Stellbereich der Servos ab. 
      posSeiten = (int)map(werte[0],0,180,0,140); // Hoher Winkel = Kippen nach Rechts
      posHoehen = (int)map(werte[1],0,180,125,0); // Hoher Winkel = Kippen zum Betrachter hin
      // Genaues Mapping hängt davon ab, wie Regler eingelernt ist (derzeit 20...160).
      leistMotor = (int)map(werte[2],0,100,20,160);
      // Empfangene Werte an den Pins ausgeben
      seitenRuder.write(posSeiten);
      hoehenRuder.write(posHoehen);
      motorRegler.write(leistMotor);
      // Rückmeldung nach jeder zehnten Kommunikation über die serielle Schnittstelle ausgeben

      // Nur zum Debuggen:
      if(leistMotor == 160) digitalWrite(13,HIGH);
      else digitalWrite(13,LOW);

      if((zaehlSensor % 10 == 0)) //Modulo Funktion (Rest nach Division)
      {
        zeitSteuerNeu = millis();
        Serial.print(kommaZahl);
        Serial.print(",");
        Serial.println((zeitSteuerNeu - zeitSteuerAlt) % 10000);
        //Serial.println(sensorWert);
        zeitSteuerAlt = zeitSteuerNeu;
      }
      zaehlSensor++;

      for(int i=0; i <= 2; i++) werte[i] = 0; // Wertearray zurücksetzen.
      arrayIndex = 0;
      kommaZahl = 0;
    }
    else
    {
      kommaZahl = 99;
    }  
  }
  if ((millis()- zeitSteuerAlt) > 2000) signalFehlt = true; // Falls seit mehr als ca. 2,5 s kein Signal mehr empfangen.
  else signalFehlt = false;
  if (signalFehlt) motorRegler.write(20); // Wenn längere Zeit kein BT-Signal mehr empfangen wurde, dann Motor ausschalten.
}
