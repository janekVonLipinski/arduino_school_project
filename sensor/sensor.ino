#define MQ_PIN (A0)                  //define which   analog input channel you are going to use
#define RL_VALUE (5)                //define the load resistance on the board, in kilo ohms
#define RO_CLEAN_AIR_FACTOR (9.83)  //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO, \
                                    //which is derived from the   chart in datasheet

/**********************Software Related Macros***********************************/
#define CALIBARAION_SAMPLE_TIMES (50)      //define how many samples you are   going to take in the calibration phase
#define CALIBRATION_SAMPLE_INTERVAL (500)  //define the time interal(in milisecond) between each samples in the \
                                           //cablibration phase
#define READ_SAMPLE_INTERVAL (50)          //define how many samples you are   going to take in normal operation
#define READ_SAMPLE_TIMES (5)              //define the time interal(in milisecond) between each samples in

/*********************Application Related Macros*********************************/
#define GAS_LPG (0)
#define GAS_CO (1)
#define GAS_SMOKE (2)

/****************************Globals**********************************************/
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

/***************************  MQGetPercentage   ********************************
Input:   rs_ro_ratio - Rs divided by Ro
          pcurve      - pointer to the curve of the target gas
Output:  ppm of   the target gas
Remarks: By using the slope and a point of the line. The x(logarithmic   value of ppm) 
         of the line could be derived if y(rs_ro_ratio) is provided.   As it is a 
         logarithmic coordinate, power of 10 is used to convert the   result to non-logarithmic 
         value.
**********************************************************************************/
float LPGCurve[3] = { 2.3, 0.21, -0.47 };    //two points are taken from the curve.
                                             //with these two points,   a line is formed which is "approximately equivalent"
                                             //to   the original curve.
                                             //data   format:{ x, y, slope}; point1: (lg200, 0.21), point2: (lg10000, -0.59)
float COCurve[3] = { 2.3, 0.72, -0.34 };     //two points are taken from the curve.
                                             //with these two points,   a line is formed which is "approximately equivalent"
                                             //to   the original curve.
                                             //data   format:{ x, y, slope}; point1: (lg200, 0.72), point2: (lg10000,  0.15)
float SmokeCurve[3] = { 2.3, 0.53, -0.44 };  //two points are taken from the curve.
                                             //with these two points,   a line is formed which is "approximately equivalent"
                                             //to   the original curve.
                                             //data   format:{ x, y, slope}; point1: (lg200, 0.53), point2: (lg10000,  -0.22)


/****************   MQResistanceCalculation **************************************
Input:   raw_adc   - raw value read from adc, which represents the voltage
Output:  the calculated   sensor resistance
Remarks: The sensor and the load resistor forms a voltage divider.   Given the voltage
         across the load resistor and its resistance, the resistance   of the sensor
         could be derived.
**********************************************************************************/
float
MQResistanceCalculation(int raw_adc) {
  return (((float)RL_VALUE * (1023 - raw_adc) / raw_adc));
}

/*************************** MQCalibration **************************************
Input:    mq_pin - analog channel
Output:  Ro of the sensor
Remarks: This function   assumes that the sensor is in clean air. It use  
         MQResistanceCalculation   to calculates the sensor resistance in clean air 
         and then divides it   with RO_CLEAN_AIR_FACTOR. RO_CLEAN_AIR_FACTOR is about 
         10, which differs   slightly between different sensors.
**********************************************************************************/
float MQCalibration(int mq_pin) {
  int i;
  float val = 0;

  for (i = 0; i < CALIBARAION_SAMPLE_TIMES; i++) {  //take multiple samples
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val / CALIBARAION_SAMPLE_TIMES;  //calculate the average   value

  val = val / RO_CLEAN_AIR_FACTOR;  //divided   by RO_CLEAN_AIR_FACTOR yields the Ro
                                    //according   to the chart in the datasheet

  return val;
}


/***************************   MQRead *******************************************
Input:   mq_pin - analog   channel
Output:  Rs of the sensor
Remarks: This function use MQResistanceCalculation   to caculate the sensor resistenc (Rs).
         The Rs changes as the sensor   is in the different consentration of the target
         gas. The sample times   and the time interval between samples could be configured
         by changing   the definition of the macros.
**********************************************************************************/
float MQRead(int mq_pin) {
  int i;
  float rs = 0;

  for (i = 0; i < READ_SAMPLE_TIMES; i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }

  rs = rs / READ_SAMPLE_TIMES;

  return rs;
}

/***************************   MQGetGasPercentage ********************************
Input:   rs_ro_ratio -   Rs divided by Ro
         gas_id      - target gas type
Output:  ppm of the   target gas
Remarks: This function passes different curves to the MQGetPercentage   function which 
         calculates the ppm (parts per million) of the target   gas.
**********************************************************************************/
int MQGetGasPercentage(float rs_ro_ratio, int gas_id) {
  if (gas_id == GAS_LPG) {
    return MQGetPercentage(rs_ro_ratio, LPGCurve);
  } else if (gas_id == GAS_CO) {
    return MQGetPercentage(rs_ro_ratio, COCurve);
  } else if (gas_id == GAS_SMOKE) {
    return MQGetPercentage(rs_ro_ratio, SmokeCurve);
  }

  return 0;
}

int MQGetPercentage(float rs_ro_ratio, float *pcurve) {
  return (pow(10, (((log10(rs_ro_ratio) - pcurve[1]) / pcurve[2]) + pcurve[0])));
}
