float Ro = 10;                               //Ro is initialized to 10 kilo ohms

int green = 2;
int yellow = 3;
int red = 4;
int buzzer_pin = 9;

int co_good = 0;
int co_ok = 200; // obviously not ok, but we tried at least
int co_bad = 16000;

#include <DHT.h>
#define DHT11_PIN 11 // define DHT pin
DHT dht(DHT11_PIN, DHT11);

const int sensorMin = 0;     //  sensor minimum for flame sensor
const int sensorMax = 1024;  // sensor maximum for flame sensor

#define ANALOG_PIN A0

void setup() {
  Serial.begin(9600);  //UART   setup, baudrate = 9600bps

  calibrate();

  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(red, OUTPUT);

  pinMode(A0, INPUT);

  dht.begin();
}


void loop() {
  //int co = readCoValue();
  //evalualteCoForTrafficLight(co);

  //Serial.println("Humidity: ");
  //float h = dht.readHumidity();
  //Serial.println(h);

  //Serial.println("Temperature: ");
  //float t = dht.readTemperature();
  //Serial.println(t);

  //readFlameSensor();

  int analog_value = analogRead(ANALOG_PIN);
  Serial.println("Analog value");
  Serial.println(analog_value);

  delay(100);
}


float calibrate() {
  Serial.print("Calibrating...");
  Ro = MQCalibration(MQ_PIN);  
  Serial.print("Calibration   is done...");
  Serial.print("Ro=");
  Serial.print(Ro);
  Serial.print("kohm");
  Serial.print("");
  return Ro;
}

int readCoValue() {
  int co = MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_CO);
  Serial.print("LPG:");
  Serial.print(MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_LPG));
  Serial.print("ppm");
  Serial.println("    ");
  Serial.print("CO:");
  Serial.print(co);  
  Serial.print("ppm");
  Serial.println("    ");
  Serial.print("SMOKE:");
  Serial.print(MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_SMOKE));
  Serial.println("ppm");
  return co;
}

void evalualteCoForTrafficLight(int co) {
  if (co < 0) {
    digitalWrite(green, HIGH);
    digitalWrite(yellow, LOW);
    digitalWrite(red, LOW);
  }

  if (co >= co_good) {
    digitalWrite(green, HIGH);
    digitalWrite(yellow, LOW);
    digitalWrite(red, LOW);
  }

  if (co >= co_ok) {
    digitalWrite(green, LOW);
    digitalWrite(yellow, HIGH);
    digitalWrite(red, LOW);
  }

  if (co >= co_bad) {
    digitalWrite(green, LOW);
    digitalWrite(yellow, LOW);
    digitalWrite(red, HIGH);
  }
}

void readFlameSensor() {
  int sensorReading = analogRead(A6);
  Serial.print("Fire???");
  Serial.println(sensorReading);

  int range = map(sensorReading,  sensorMin, sensorMax, 0, 3);

  switch (range) {
    case 0:    // A fire closer than 1.5 feet away.
      Serial.println("** Close  Fire **");
      break;
    case 1:    // A fire between 1-3 feet away.
      Serial.println("**  Distant Fire **");
      break;
    case 2:    // No fire detected.
      Serial.println("No  Fire");
      break;
  }
}
