#include <Keypad.h>   //Hier wird die größe des Keypads definiert

const byte COLS = 4;  //4 Spalten

const byte ROWS = 4;  //4 Zeilen //Die Ziffern und Zeichen des Keypads werden eingegeben:

char hexaKeys[ROWS][COLS] = {

        { 13, 15, 0, 16 },
        { 12, 9, 8, 7 },
        { 11, 6, 5, 4 },
        { 10, 3, 2, 1 }
};

byte colPins[COLS] = { 2, 3, 4, 5 };                                             //Definition der Pins für die 4 Spalten

byte rowPins[ROWS] = { 6, 7, 8, 9 };                                             //Definition der Pins für die 4 Zeilen

int Taste;                                                                      //Taste ist die Variable für die jeweils gedrückte Taste.

Keypad Tastenfeld = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);  

//Das Keypad kann absofort mit "Tastenfeld" angesprochen werden.

 

void setup() {

  Serial.begin(9600);
  pinMode(25, OUTPUT);
}

void loop() {

  Taste = Tastenfeld.getKey();  //Unter der Variablen „Taste“ wird die gedrückte Taste gespeichert.

 if (Taste) {      //Wenn eine Taste gedrückt wurde Serial.print("Die Taste ");
    
    
    digitalWrite(25, HIGH);
    delay(Taste * 50);
    digitalWrite(25, LOW);
    Serial.println(Taste);        // Kurze Pause, damit eine Taste nicht mehrfach angezeigt wird
  }

}
