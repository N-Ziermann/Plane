// Bibliothek für die serielle Kommunikation einbinden.
import processing.serial.*;  
// Serielle Schnittstelle (USB-Verbindung zum Arduino)
Serial myPort;         
float abstand;
// Abschlusszeichen, wenn Arduino etwas über die Schnittstelle überträgt.
int linefeed = 10; 
String echoArduino = "";

void setup()
{
  size(400, 400);
  background(255, 255, 0);
  // Alle verfügbaren Schnittstellen auflisten.
  println(Serial.list());

  // Hier die richtige COM Port Nummer angeben
  // Dafür in der Liste in der Console nachschauen.
  myPort = new Serial(this, Serial.list()[0], 9600);
  textSize(24);
}

void draw()
{
  if (mousePressed) {
    background(255, 255, 0);
    abstand = dist(mouseX, mouseY, 200, 200);
    if (abstand <50) {
      fill(255, 0, 0);
      ellipse(200, 200, 100, 100);
      myPort.write("LED AN");
      myPort.write('\n');
    }
    else {
      fill(0, 255, 0);
      ellipse(200, 200, 100, 100);
      myPort.write("LED AUS");
      myPort.write('\n');
    }
    // Die verfügbaren ASCII-Zeichen aus der seriellen Schnittstelle
    // auslesen bis zum Zeichen "linefeed" (ASCII-Zeichen 10)
    echoArduino = myPort.readStringUntil(linefeed);
    // Zeichenfarbe auf Schwarz stellen
    fill(0, 0, 0);
    // Die oben rückgelesene Nachricht vom Arduino im Text an der 
    // xy-Position (50,100) darstellen.
    text("Arduino: " + echoArduino, 50, 100);
  }
}

