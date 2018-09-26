
/*
  FeedTemperatureSignal.ino

  This example code that shows how to feed a signal using the circusofthings.com API through its CircusWifiLib library for Arduino.

  With this code you can feed a signal taking the values read from a temperatures sensor. 
  
  In this case, we assume we have a DHT11 sensor that is controlled by a propietary protocol implemented in its specific library.

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
char analogSignalTemperatureKey[] = "your_temp_signal_key_here";    // The key of the signal you have created at circusofthings.com

SoftwareSerial ss(TXPinForWifiModule,RXPinForWifiModule);
WiFiEspClient client;
CircusWifiLib circus(Serial,&ss,&client,wifiSerialBaudRate,ssid,pass,token);

// ------------------------------------------------
// These are the Example related declarations
// ------------------------------------------------

#define DHTPIN 5      // digital for serial propietary portocol of sensor DHT11
#define DHTTYPE DHT11 // exact model of temperature sensor DHT 11 for the general library

DHT dht(DHTPIN, DHTTYPE);



void setup() {
  Serial.begin(debugSerialBaudRate);
  ss.begin(wifiSerialBaudRate);
  dht.begin();
  circus.begin();  
}

void loop() {
  
  float t = dht.readTemperature();
  if (isnan(t)) {
    t=-1; // if so, check the connection of your DHT11 sensor
  }
  
  circus.write(analogSignalTemperatureKey,t);
  delay(3000);
  
}
