/*
  ReadSingleSignal.ino

  This example code that shows how to read a signal using the circusofthings.com API through its CircusWifiLib library for Arduino.

  With this code it will read random signal existing at Circus and turn on/off some leds acording to it.

  A software serial port is used, so the onboard serial port is used to monitor the process.

  These will be the 3rd party libraries you will need to download:
      ArduinoJson Built in by Benoit Blanchon Version 5.13.2
      WiFiEsp by bportulari Version 2.2.2
  
  Created by Jaume Miralles Isern, September 26, 2018.
*/


#include <CircusWifiLib.h>

int TXPinForWifiModule = 2;               // IO port in your arduino you will use as TX for serial communication with the wifi module
int RXPinForWifiModule = 3;               // IO port in your arduino you will use as RX for serial communication with the wifi module
char ssid[] = "your_wifi_SSID_here";      // your wifi network SSID
char pass[] = "your_wifi_password_here";  // your wifi network password
int wifiSerialBaudRate = 9600;            // Baud rate between your arduino and your wifi module. Did you check that your module actually uses this baud rate?
int debugSerialBaudRate = 9600;           // Baud rate between your arduino and your computer
char token[] = "your_user_token_here";    // Your API token for Circus
char key[] = "23780";                     // This is the key for the signal 'Random Signal' available in the Community Feed @ circusofthings.com

SoftwareSerial ss(TXPinForWifiModule,RXPinForWifiModule);
WiFiEspClient client; 
CircusWifiLib circus(Serial,&ss,&client,wifiSerialBaudRate,ssid,pass,token);

void setup() {
  Serial.begin(debugSerialBaudRate);
  pinMode(6, OUTPUT); // It will light if the value of the read signal is lower than 30
  pinMode(7, OUTPUT); // It will light if the value of the read signal is between 30 and 60
  pinMode(8, OUTPUT); // It will light if the value of the read signal is higher than 60
  pinMode(9, OUTPUT); // It will light if there is server connection to circusofthings.com
}

void loop() {
  if (circus.circusServerStatus()) {digitalWrite(9, HIGH);} else {digitalWrite(9, LOW);}
  double value = circus.read(key);     
  decide(value);      // decide what leds turn on or off
}

void decide(double value) {
  if(value<30) {
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
  }
  if(value>30&&value<60) {
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
    digitalWrite(8, LOW);
  }
  if(value>60) {
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(8, HIGH);
  }
}
