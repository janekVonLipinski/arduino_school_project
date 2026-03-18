#include <SPI.h>
#include <WiFi101.h>
#include <SoftwareSerial.h>

WiFiServer server(80);
SoftwareSerial ESP8266(18, 19);  // RX, TX

int gas_pin = A0;

int status = WL_IDLE_STATUS;

unsigned char check_connection = 0;

int array_length = 64;
int myData[64];

unsigned long previousMeasureMenttime = millis();
unsigned long previousServerTime = millis();

long gasReadingdurationInMillis = 10000;
int serverDurationInMillis = 100;

int counter = 0;


void setup() {

  pinMode(gas_pin, INPUT);

  Serial.begin(115200);   // USB -> PC
  Serial1.begin(115200);  // Mega ↔ ESP-01 (TX1=18, RX1=19)

  delay(1000);
  Serial.println("Initializing ESP-01 HTTP Server...");

  // Test ESP-01
  sendAT("AT", 1000);

  // Set WiFi mode to Station+AP (1=Station, 2=AP, 3=Both)
  sendAT("AT+CWMODE=3", 1000);

  // Connect to WiFi (replace with your SSID & password)
  sendAT("AT+CWJAP=\"carrambahotspotayayaymuyinternet\",\"tollesfeineshotspotpasswort\"", 10000);

  // Enable multiple connections
  sendAT("AT+CIPMUX=1", 1000);

  // Start TCP server on port 80
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
  }
}

void measure_gas() {
  int gas = analogRead(gas_pin);

  if (counter < array_length) {
    myData[counter++] = gas;
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
