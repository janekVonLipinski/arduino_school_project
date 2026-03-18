#include <WiFi101.h>
#include <SoftwareSerial.h>

WiFiServer server(80);
SoftwareSerial ESP8266(18, 19);  // RX, TX

int gas_pin = A0;
int green = 2;
int yellow = 3;
int red = 4;

int status = WL_IDLE_STATUS;

int array_length = 1024;
int myData[1024];

unsigned long previousMeasureMenttime = millis();
unsigned long previousServerTime = millis();

long gasReadingdurationInMillis = 1000;
int serverDurationInMillis = 100;

unsigned char check_connection = 0;
int counter = 0;
int valuesMeasured = 0;
int sumOfGas = 0;
int gas = 0;

void setup() {

  pinMode(gas_pin, INPUT);
  pinMode(green, INPUT);
  pinMode(yellow, INPUT);
  pinMode(red, INPUT);

  Serial.begin(115200);
  Serial1.begin(115200);

  delay(1000);
  Serial.println("Initializing ESP-01 HTTP Server...");

  sendAT("AT", 1000);

  sendAT("AT+CWMODE=3", 1000);

  sendAT("AT+CWJAP=\"myHotspot\",\"myPassword\"", 10000);

  sendAT("AT+CIPMUX=1", 1000);

  sendAT("AT+CIPSERVER=1,80", 1000);
  sendAT("AT+CIFSR", 1000);

  if (Serial1.available()) {
  }

  Serial.println("ESP-01 HTTP Server started on port 80.");
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - previousServerTime > serverDurationInMillis) {
    previousServerTime = currentTime;
    handle_webserver();
  }

  if (currentTime - previousMeasureMenttime > gasReadingdurationInMillis) {
    previousMeasureMenttime = currentTime;

    measure_gas();
    handle_mr_nolting_gedenk_ampel();
  }
}



void measure_gas() {
  gas = analogRead(gas_pin);
  sumOfGas += gas;
  valuesMeasured++;

  if (valuesMeasured == 10) {
    sumOfGas = sumOfGas / 10;
    myData[counter++] = sumOfGas;
    valuesMeasured = 0;
    sumOfGas = 0;
  }
}

void handle_webserver() {

  if (!Serial1.available()) return;

  String data = "";

  unsigned long readTimeout = millis();
  
  while (millis() - readTimeout < 200) {
    while (Serial1.available()) {
      char c = Serial1.read();
      data += c;
      readTimeout = millis();
    }
  }

  if (data.length() == 0) return;

  Serial.println("---- ESP RX ----");
  Serial.println(data);
  Serial.println("----------------");

  int idStart = data.indexOf("+IPD,") + 5;
  int idEnd = data.indexOf(",", idStart);
  
  if (idStart < 5 || idEnd == -1) return;

  int connectionId = data.substring(idStart, idEnd).toInt();

  String httpHeader =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: application/json\r\n"
    "Connection: close\r\n"
    "\r\n";

  String httpBody = convertArrayToJson();
  Serial.println(httpBody);

  String httpResponse = httpHeader + httpBody;

  Serial1.print("AT+CIPSEND=");
  Serial1.print(connectionId);
  Serial1.print(",");
  Serial1.println(httpResponse.length());

  delay(50); //böse, das muss weg
  Serial1.print(httpResponse);

  Serial1.print("AT+CIPCLOSE=");
  Serial1.println(connectionId);
}

void sendAT(String cmd, int timeout) {
  Serial1.println(cmd);
  long start = millis();
  while (millis() - start < timeout) {
    while (Serial1.available()) {
      Serial.write(Serial1.read());
    }
  }
  Serial.println();
}

String convertArrayToJson() {
  String res = "{";

  for (int i = 0; i < counter; i++) {
    res += "\"" + String(i) + "\":" + String(myData[i]);

    if (i < counter - 1) {
      res += ",";
    }
  }

  res += "}";
  return res;
}

void handle_mr_nolting_gedenk_ampel() {
  
  if (gas <= 620) {
    
    digitalWrite(green, HIGH);
    digitalWrite(yellow, LOW);
    digitalWrite(red, LOW);
  }

  if (gas > 620) {
    digitalWrite(yellow, HIGH);
    digitalWrite(red, LOW);
    digitalWrite(green, LOW);
  }

  if (gas > 720) {
    digitalWrite(red, HIGH);
    digitalWrite(yellow, LOW);
    digitalWrite(green, LOW);
  }
}