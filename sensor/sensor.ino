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

  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(red, OUTPUT);

  pinMode(A0, INPUT);

  dht.begin();
  Serial.println("Waiting for gas sensor to heat up");
  delay(60000);
}


void loop() {
  Serial.println("Humidity: ");
  float h = dht.readHumidity();
  Serial.println(h);

  Serial.println("Temperature: ");
  float t = dht.readTemperature();
  Serial.println(t);

  //readFlameSensor();

  int analog_value = analogRead(ANALOG_PIN);
  Serial.println("Analog value");
  Serial.println(analog_value);

  int avg = (analog_value); //+ analog_value_2) / 2; 

  //evalualteGasConcentrationForTrafficLight(avg);

  producer_code();

  delay(2000);
}

void producer_code() {
      float sensor_volt;
    float sensorValue;
    float RS_air;
    float R0;
    float RS_gas;
    float ratio;
    
    sensorValue = analogRead(A0);
    sensor_volt = sensorValue/1024*5.0;
    
    for(int x = 0 ; x < 100 ; x++)
    {
        sensorValue = sensorValue + analogRead(A0);
    }
    sensorValue = sensorValue/100.0;

    sensor_volt = sensorValue/1024*5.0;
    RS_air = (5.0-sensor_volt)/sensor_volt;
    R0 = RS_air/9.8;
    RS_gas = (5.0-sensor_volt)/sensor_volt;
    ratio = RS_gas/R0;

    Serial.print("R0 = ");
    Serial.println(R0);
    
    Serial.print("sensor_volt = ");
    Serial.println(sensor_volt);
    Serial.print("RS_ratio = ");
    Serial.println(RS_gas);
    Serial.print("Rs/R0 = ");
    Serial.println(ratio);

    Serial.print("\n\n");
}


void evalualteGasConcentrationForTrafficLight(int analog_value) {
  int schwelle1 = 900;
  int schwelle2 = 1200;
  
  if (analog_value < 0) {
    return;
  }

  if (analog_value <= schwelle1) {
    digitalWrite(green, HIGH);
    digitalWrite(yellow, LOW);
    digitalWrite(red, LOW);
    return;
  }

  if (analog_value <= schwelle2) {
    digitalWrite(green, LOW);
    digitalWrite(yellow, HIGH);
    digitalWrite(red, LOW);
    return;
  }

  digitalWrite(green, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(red, HIGH);
  
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
