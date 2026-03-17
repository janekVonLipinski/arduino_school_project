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


#include <SPI.h>
#include <WiFi101.h>
#include <SoftwareSerial.h>

char ssid[] = "hotspotswag420blazeit";      // your network SSID (name)
char pass[] = "tollesfeineshotspotpasswort";   // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(80);
SoftwareSerial ESP8266(18, 19); // RX, TX
unsigned char check_connection = 0;

void setup() {
  Serial.begin(115200);    // USB -> PC
  Serial1.begin(115200);   // Mega ↔ ESP-01 (TX1=18, RX1=19)

  delay(1000);
  Serial.println("Initializing ESP-01 HTTP Server...");

  // Test ESP-01
  sendAT("AT", 1000);

  // Set WiFi mode to Station+AP (1=Station, 2=AP, 3=Both)
  sendAT("AT+CWMODE=3", 1000);

  // Connect to WiFi (replace with your SSID & password)
  sendAT("AT+CWJAP=\"hotspotswag420blazeit\",\"tollesfeineshotspotpasswort\"", 10000);

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
  // Forward any data from ESP to Serial Monitor
  if (Serial1.available()) {
    String data = "";
    while (Serial1.available()) {
      char c = Serial1.read();
      data += c;
    }

    Serial.print(data); // Print incoming ESP data

    // Detect a connection request (usually "+IPD" message)
    int ipdIndex = data.indexOf("+IPD,");
    if (ipdIndex != -1) {
      // Extract connection ID
      int idIndex = data.indexOf(',', ipdIndex + 5);
      String connID = data.substring(ipdIndex + 5, idIndex);

      // Send HTTP response
      String httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
      httpResponse += "<html><body><h1>Hello from ESP-01!</h1></body></html>";

      String cmd = "AT+CIPSEND=" + connID + "," + String(httpResponse.length());
      sendAT(cmd, 1000);         // Tell ESP how many bytes we will send
      sendAT(httpResponse, 1000); // Send the actual HTTP response

      // Close the connection
      sendAT("AT+CIPCLOSE=" + connID, 1000);
    }
  }

  // Forward any Serial Monitor input to ESP (optional)
  if (Serial.available()) {
    Serial1.write(Serial.read());
  }
}

// Helper function to send AT commands and print response
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
/*
void setup() {
  Serial.begin(115200);
  ESP8266.begin(115200);
  ESP8266.print("***");
  delay(1000);
}
  //wifi_setup();

  Serial.begin(9600);  //UART   setup, baudrate = 9600bps

  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(red, OUTPUT);

  pinMode(A0, INPUT);

  dht.begin();
  Serial.println("Waiting for gas sensor to heat up");
  delay(60000);
*/

/*
void loop() {
  
  Serial.println("Connecting to WIFI");
  
  while (check_connection==0) {
    Serial.print("Hi Kevin");

    ESP8266.print("AT+CWMODE=1");
    ESP8266.print("AT+CWJAP=\"BISON X10\",\"kwju9x95wmzx38q\"\r\n");
    ESP8266.setTimeout(5000);

    if (ESP8266.find("WIFI CONNECTED\r\n") == 1) {
      Serial.println("WIFI CONNECTED");
      break;
    }

    while(1);

  }

  Serial.print("Ich bin dir sehr verbunden");
}
*/
  //wifi_loop();
/*
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

  delay(2000); */

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

void wifi_setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
}


void wifi_loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}