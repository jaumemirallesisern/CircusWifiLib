
/*
  FeedMultipleSignals.ino 

  This example code that shows how to feed a few signals using the circusofthings.com API through its CircusWifiLib library for Arduino.

  I used this code to feed five of my pusblished signals that can be seen in the Community Feed @ circusofthings.com:
      - 'Is it rainning in Madrid?'
      - 'Rain level in Madrid (at Puente de Segovia)'
      - 'Temperature in Madrid (at Puente de Segovia)'
      - 'Is Elisas plant soil wet?'
      - 'Elisas plant soil moisture level'

  My sensors are connected to my Arduino Nano board:
      - A rain sensor MH-RD, that i will use to feed a signal telling whether it's rainning or not (decission depends on manually adjusted potentiometer) and an analog one to report the intensity of the rain.
      - A soil moisture sensor YL-38, that i will use to feed a signal telling whether soil is wet or not (decission depends on manually adjusted potentiometer) and an analog one to report the degree of moisture.
      - An ambient temperature sensor, that will feed the actual temperature. It's controlled by a propietary protocol.

  A software serial port is used, so the onboard serial port is used to monitor the process.

  These will be the 3rd party libraries you will need to download:
      DHT Sensor Library Built in by Adafruit Version 1.3.0 
      ArduinoJson Built in by Benoit Blanchon Version 5.13.2
      WiFiEsp by bportulari Version 2.2.2
  
  Created by Jaume Miralles Isern, September 26, 2018.
*/


#include <CircusWifiLib.h>

// ------------------------------------------------
// These are the CircusWifiLib related declarations
// ------------------------------------------------

int TXPinForWifiModule = 2;               // IO port in your arduino you will use as TX for serial communication with the wifi module
int RXPinForWifiModule = 3;               // IO port in your arduino you will use as RX for serial communication with the wifi module
char ssid[] = "your_wifi_SSID_here";      // your wifi network SSID
char pass[] = "your_wifi_password_here";  // your wifi network password
int wifiSerialBaudRate = 9600;            // Baud rate between your arduino and your wifi module. Did you check that your module actually uses this baud rate?
int debugSerialBaudRate = 9600;           // Baud rate between your arduino and your computer
char token[] = "your_user_token_here";    // Your API token for Circus
char digitalSignalRainKey[] = "1028182812";         // The key for my signal published at circusofthings.com 'Is it rainning in Madrid?'
char analogSignalRainKey[] = "830339759";           // The key for my signal published at circusofthings.com 'Rain level in Madrid (at Puente de Segovia)'
char analogSignalTemperatureKey[] = "943874351";    // The key for my signal published at circusofthings.com 'Temperature in Madrid (at Puente de Segovia)'
char digitalSignalSoilMoistureKey[] = "443905254";  // The key for my signal published at circusofthings.com 'Is Elisas plant soil wet?'
char analogSignalSoilMoistureKey[] = "196601482";   // The key for my signal published at circusofthings.com 'Elisas plant soil moisture level'

SoftwareSerial ss(TXPinForWifiModule,RXPinForWifiModule);
WiFiEspClient client; 
CircusWifiLib circus(Serial,&ss,&client,wifiSerialBaudRate,ssid,pass,token);

// ------------------------------------------------
// These are the Example related declarations
// ------------------------------------------------

#define ARAIN A0      // analog read for the rain sensor MH-RD
#define DRAIN 6       // digital read for the rain sensor MH-RD
#define DHTPIN 5      // digital for serial propietary portocol of sensor DHT11
#define ASOIL A1      // analog read for the soil moisture sensor YL-38
#define DSOIL 4       // digital read for the soil moisture sensor YL-38
#define DHTTYPE DHT11 // exact model of temperature sensor DHT 11 for the general library

DHT dht(DHTPIN, DHTTYPE);
double rainValue = 0;
double rainState = 0;
double soilValue = 0;
double soilState = 0;



void setup() {
  Serial.begin(debugSerialBaudRate);
  ss.begin(wifiSerialBaudRate);
  dht.begin();
  circus.begin();
  pinMode(DRAIN, INPUT);
  pinMode(DHTPIN, INPUT);
  pinMode(DSOIL, INPUT);  
}

void loop() {
  
  soilValue = percentageHummidityConversion((float)analogRead(ASOIL));
  soilState = !digitalRead(DSOIL); // notice I do a 'not' function, so now dry is represented by 0, and wet by 1
  rainValue = trimConversion(percentageHummidityConversion((float)analogRead(ARAIN)));
  rainState = !digitalRead(DRAIN); // notice I do a 'not' function, so now dry is represented by 0, and wet by 1
  float t = dht.readTemperature();
  if (isnan(t)) {
    t=-1; // if so, check the connection of your DHT11 sensor
  }
  
  delay(3000);
  circus.write(digitalSignalRainKey,rainState); 
  delay(3000);
  circus.write(analogSignalRainKey,rainValue);
  delay(3000);
  circus.write(analogSignalTemperatureKey,t);
  delay(3000);
  circus.write(digitalSignalSoilMoistureKey,soilState);
  delay(3000);
  circus.write(analogSignalSoilMoistureKey,soilValue);
  
}



// ------------------------------------------------
// Auxiliar functions for my Example
// ------------------------------------------------

float percentageHummidityConversion(float roughValue){
  // I checked that my sensor goes from values 300 (submerged in water) to 1023 (completely dry)
  // I prefer posting a percentage value, where 0% means dry, and 100% completely wet, so I do the scale adjusting with this function
  float minSensorValue = 300;
  float maxSensorValue = 1023;
  float percentValue = 100 - 100 * (roughValue-minSensorValue) / (maxSensorValue-minSensorValue);
  return percentValue;
}

float trimConversion(float roughValue){
  // Yet another consideration for rain
  // my sensor is actually reading hummidity in the air. Levels below 25% doesn't mean I've got real rain drops on it
  float infoValue = roughValue;
  if (roughValue<25) {infoValue=0;}
  return infoValue;
}
