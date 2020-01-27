char einByte = 0; // ankommendes Byte
String zeichenInput = ""; // Puffer für ankommende Bytefolgen
String zeichenLEDan = "LED AN"; // String mit dem "Codewort";
String zeichenLEDaus = "LED AUS"; // String mit dem "Codewort";

void setup() {
  pinMode(13, OUTPUT); // Pin 13 mit der LED als Ausgang kofigurieren.
  Serial.begin(9600); // Serielle Schnittstelle auf 9600 Baud einstellen.
}

void loop() {
  // Solange etwas an der seriellen Schnittstelle anliegt...
  while (Serial.available() > 0) {
    einByte = Serial.read(); // ... jeweils ein einzelnes Byte lesen.
    if (einByte == '\n') //Wenn ein "Neue Zeile"-Zeichen kommt...
    {
      if(zeichenInput.equals(zeichenLEDan)) // Bei Übereinstimmung LED einschalten.
      {
        digitalWrite(13, HIGH);
        Serial.println(" LED ist an!"); // LED Status über Schnittstelle zurück melden.
      }
      if(zeichenInput.equals(zeichenLEDaus)) // Bei Übereinstimmung LED ausschalten.
      {
        digitalWrite(13, LOW);
        Serial.println(" LED ist aus!"); // LED Status über Schnittstelle zurück melden.
      }
      zeichenInput = ""; // Puffer wieder leer machen
    }
    else
    {
      //Wenn das Endzeichen noch nicht da, neues Zeichen an die vorhandenen anhängen.
      zeichenInput += einByte;
    }
  }
}

